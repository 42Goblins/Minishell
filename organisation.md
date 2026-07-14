# Organisation Minishell

Ce document sert de plan de travail pour ne pas partir dans tous les sens.
Le but n'est pas de faire Bash d'un coup, mais d'avancer par petites versions
qui marchent.

Chaine generale du projet:

```text
readline -> lexer -> parser -> expansion -> execution -> cleanup
```

Pour le projet obligatoire, on ignore les bonus au debut:

- pas de `&&`
- pas de `||`
- pas de parentheses
- pas de wildcard `*`

La reference quand un comportement n'est pas clair: tester dans `bash`.

## 1. Repartition toi / Dounia

### Toi: parsing

Tu t'occupes de transformer la ligne tapee par l'utilisateur en structure claire
pour l'execution.

Responsabilites:

- lexer / tokenisation
- gestion des quotes simples `'` et doubles `"`
- detection des pipes `|`
- detection des redirections `<`, `>`, `<<`, `>>`
- erreurs de syntaxe avant execution
- expansion des variables `$VAR` et `$?`
- suppression des quotes quand elles ne doivent plus apparaitre
- construction de la structure `t_cmd`
- fonctions de debug pour afficher les tokens et les commandes
- fonctions de free pour les structures que tu crees

Livrables principaux:

```c
t_token	*lexer(char *line);
t_cmd	*parser(t_token *tokens);
int		expand_cmds(t_cmd *cmds, t_env *env, int last_status);
void	free_tokens(t_token *tokens);
void	free_cmds(t_cmd *cmds);
```

### Dounia: execution

Dounia s'occupe d'utiliser la structure produite par le parsing pour vraiment
lancer les commandes.

Responsabilites:

- boucle principale avec `readline`
- copie et gestion de l'environnement
- builtins obligatoires
- recherche des executables avec `PATH`
- execution avec `fork`, `execve`, `waitpid`
- creation et connexion des pipes avec `pipe` et `dup2`
- application systeme des redirections avec `open` et `dup2`
- heredoc cote lecture/execution
- signaux
- mise a jour du dernier exit status
- frees des structures qu'elle cree

Livrables principaux:

```c
int		shell_loop(t_shell *shell);
int		execute_pipeline(t_cmd *cmds, t_shell *shell);
int		run_builtin(t_cmd *cmd, t_shell *shell);
char	**env_to_array(t_env *env);
```

### A faire ensemble

Vous devez vous mettre d'accord tot sur le contrat entre parsing et execution.
Si ce contrat change tout le temps, vous allez vous bloquer.

Decisions communes:

- structs exactes dans `include/minishell.h`
- qui appelle `lexer`, `parser`, `expand_cmds`, `execute_pipeline`
- qui free chaque structure
- format des erreurs
- valeur de retour des fonctions
- comment stocker `env` et `last_status`
- affichage debug temporaire des tokens et commandes

### Contrat v1 simple a proposer a Dounia

Vous n'avez pas besoin d'avoir les structs parfaites des le debut. Par contre,
il faut une version v1 assez stable pour que Dounia puisse coder l'exec sans
attendre que tout le parsing soit fini.

Objectif du contrat:

- toi tu transformes une ligne en liste de `t_cmd`
- Dounia execute uniquement cette liste de `t_cmd`
- Dounia ne repars pas de la string brute pour deviner les pipes/redirections
- si le parsing trouve une erreur, l'exec n'est pas appelee

Interface simple possible:

```c
int	parse_line(char *line, t_shell *shell, t_cmd **cmds);
int	execute_pipeline(t_cmd *cmds, t_shell *shell);
```

Comportement attendu:

- `parse_line` renvoie `0` si une commande est prete
- `parse_line` renvoie `1` si la ligne est vide et qu'il n'y a rien a executer
- `parse_line` renvoie `2` si erreur de syntaxe, par exemple `|` ou `echo >`
- `parse_line` renvoie `-1` si erreur malloc
- si `parse_line` ne renvoie pas `0`, Dounia n'appelle pas l'exec

Pour discuter simplement avec Dounia, tu peux presenter le flow comme ca:

```text
line
-> parse_line()
-> t_cmd *cmds
-> execute_pipeline(cmds, shell)
-> free_cmds(cmds)
```

### Ce que Dounia peut supposer en v1

Si `execute_pipeline` recoit une liste de `t_cmd`, Dounia peut supposer que:

- chaque maillon `t_cmd` correspond a une commande entre deux pipes
- `cmd->next` veut dire qu'il y a un pipe vers la commande suivante
- `cmd->argv` est un tableau termine par `NULL`, comme attendu par `execve`
- `cmd->argv[0]` est le nom de la commande ou du builtin
- les redirections ne sont pas dans `argv`
- les redirections sont dans `cmd->redirs`
- les redirections sont dans l'ordre ou elles apparaissent dans la ligne
- les quotes de syntaxe ont deja ete retirees
- les variables ont deja ete expand quand il faut

Exemple:

```sh
cat < input | grep hello > out
```

Doit devenir:

```text
cmd 1:
  argv: ["cat", NULL]
  redirs: [R_IN -> "input"]
  next: cmd 2

cmd 2:
  argv: ["grep", "hello", NULL]
  redirs: [R_OUT -> "out"]
  next: NULL
```

Avec ce contrat, Dounia peut deja coder:

- execution d'une commande simple avec `argv`
- detection builtin vs executable
- pipes en suivant `cmd->next`
- redirections en parcourant `cmd->redirs`
- tests d'exec en creant une `t_cmd` a la main meme si ton parser n'est pas fini

### Ce qui peut rester provisoire

Vous pouvez accepter que certaines choses changent plus tard:

- le nom exact de certaines fonctions
- l'ajout de champs dans `t_shell`
- des helpers de debug
- des fonctions de free plus propres
- les details de heredoc

Par contre, evitez de changer toutes les deux heures:

- le sens de `t_cmd`
- le fait que `cmd->next` represente les pipes
- le fait que `argv` soit un tableau termine par `NULL`
- le fait que les redirections soient separees dans `t_redir`
- les noms des champs principaux: `argv`, `redirs`, `next`, `type`, `target`

Donc oui, il faut des structs assez exactes pour que Dounia commence, mais elles
peuvent etre une version v1. Le plus important est que vous soyez d'accord sur
ce que l'exec a le droit de lire et sur ce que le parsing promet de fournir.

## 2. Lexique simple du parsing

### Token

Un token est un petit morceau reconnu dans la ligne.

Exemple:

```sh
echo "hello $USER" | grep h > out.txt
```

Tokenisation:

```text
WORD(echo)
WORD(hello $USER)
PIPE
WORD(grep)
WORD(h)
REDIR_OUT
WORD(out.txt)
```

Les espaces servent souvent a separer les tokens, sauf quand ils sont proteges
par des quotes.

### Lexer / tokenisation

Le lexer lit la chaine brute caractere par caractere et produit une liste de
tokens.

Il doit reconnaitre:

- les mots: `echo`, `hello`, `$HOME`
- les pipes: `|`
- les redirections: `<`, `>`, `<<`, `>>`
- les quotes ouvertes/fermees
- les erreurs simples comme une quote non fermee

Le lexer ne lance aucune commande. Il ne fait que decouper proprement.

### Parser

Le parser prend la liste de tokens et construit des commandes.

Il doit comprendre que:

- `|` separe deux commandes
- `<`, `>`, `<<`, `>>` ne sont pas des arguments
- le mot apres une redirection est le fichier ou le delimiter
- les autres mots deviennent `argv`

Exemple:

```sh
cat < input | grep hello > out
```

Resultat logique:

```text
cmd 1:
  argv: ["cat"]
  redirs: [IN -> input]

cmd 2:
  argv: ["grep", "hello"]
  redirs: [OUT -> out]
```

### Expansion

L'expansion remplace les variables par leur valeur.

Exemples:

```sh
echo $HOME
echo "$HOME"
echo '$HOME'
echo $?
```

Regles importantes:

- `$HOME` devient la valeur de `HOME`
- `$?` devient le dernier exit status
- dans les single quotes, on ne remplace pas les variables
- dans les double quotes, on remplace les variables
- a la fin, les quotes de syntaxe ne doivent plus etre dans `argv`

L'expansion est cote parsing, mais elle a besoin d'informations gardees par le
core/execution: l'environnement et le dernier exit status.

### Structure de commandes

Pour le Minishell obligatoire, on ne prevoit pas de faire les bonus. Donc on n'a
pas besoin de construire un vrai arbre syntaxique complexe.

Les bonus comme `&&`, `||`, les parentheses et les priorites demanderaient une
structure plus avancee. Comme on ne les fait pas, le contrat le plus simple est
une liste chainee de commandes:

```text
t_cmd -> t_cmd -> t_cmd
```

Chaque `t_cmd` represente une commande entre deux pipes. Si `cmd->next` existe,
ca veut dire qu'il y a un pipe vers la commande suivante.

Exemple:

```sh
echo "hello $USER" | grep h > out.txt
```

Structure attendue:

```text
cmd 1:
  argv: ["echo", "hello <valeur_de_USER>", NULL]
  redirs: []
  next: cmd 2

cmd 2:
  argv: ["grep", "h", NULL]
  redirs: [R_OUT -> "out.txt"]
  next: NULL
```

Les redirections sont attachees a la commande concernee, pas gardees comme des
arguments dans `argv`.

## 3. Contrat parsing -> execution

Le parsing doit donner a l'execution une structure deja propre.

L'execution ne doit pas avoir a deviner la syntaxe. Elle doit juste appliquer ce
que le parser a compris.

### Ce que le parser donne a l'exec

- `argv` sans quotes inutiles
- redirections separees des arguments
- une commande par maillon `t_cmd`
- les pipes representes par `cmd->next`
- expansion deja faite quand elle doit etre faite
- erreurs de syntaxe detectees avant de lancer quoi que ce soit

### Structures communes proposees

```c
typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_APPEND,
	T_HEREDOC
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;
```

```c
typedef enum e_redir_type
{
	R_IN,
	R_OUT,
	R_APPEND,
	R_HEREDOC
}	t_redir_type;

typedef struct s_redir
{
	t_redir_type	type;
	char			*target;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	char			**argv;
	t_redir			*redirs;
	struct s_cmd	*next;
}	t_cmd;
```

```c
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_shell
{
	t_env	*env;
	int		last_status;
	int		interactive;
}	t_shell;
```

Ces structs peuvent changer, mais il faut les stabiliser rapidement avant de
coder trop loin.

## 4. Workflow precis

### Phase 0: squelette projet

Objectif:

- creer `Makefile`
- garder `include/minishell.h`
- creer les dossiers `src/`, `src/parsing/`, `src/exec/`, `src/builtins/`
- compiler avec `-Wall -Wextra -Werror`
- lier `libft`
- ajouter `readline` quand la boucle existe

Validation:

```sh
make
./minishell
```

### Phase 1: structures communes

Objectif:

- definir `t_shell`
- definir `t_env`
- definir `t_token`
- definir `t_redir`
- definir `t_cmd`
- ajouter les prototypes principaux dans le `.h`
- ajouter les fonctions de free

Validation:

- le projet compile
- chaque structure a une fonction de free prevue

### Phase 2: boucle readline minimale

Responsable principal: Dounia.

Objectif:

- afficher un prompt
- lire une ligne avec `readline`
- quitter proprement avec `Ctrl-D`
- ajouter les lignes non vides a l'historique
- stocker `last_status`

Validation:

```sh
./minishell
minishell$ echo test
minishell$ Ctrl-D
```

A ce stade, la ligne peut juste etre affichee ou ignoree.

### Phase 3: lexer simple

Responsable principal: toi.

Objectif:

- decouper les mots simples separes par espaces
- reconnaitre `|`
- reconnaitre `<`, `>`, `<<`, `>>`
- creer une liste `t_token`
- afficher les tokens en debug

Tests:

```sh
echo hello
ls -la
cat < input
echo hi > out
echo hi >> out
ls | wc -l
cat << EOF
```

### Phase 4: parser simple

Responsable principal: toi.

Objectif:

- transformer `t_token` en `t_cmd`
- remplir `argv`
- attacher les redirections a la bonne commande
- creer une nouvelle `t_cmd` apres chaque pipe
- afficher les commandes en debug

Tests:

```sh
echo hello
cat < input
echo hello > out
ls | wc -l
cat < input | grep hello > out
```

### Phase 5: quotes

Responsable principal: toi.

Objectif:

- garder les espaces dans les quotes
- detecter les quotes non fermees
- retirer les quotes de syntaxe a la fin
- faire attention aux morceaux colles comme `hello"world"`

Tests:

```sh
echo "hello world"
echo 'hello world'
echo hello"world"
echo "a b"c
echo "unclosed
echo 'unclosed
```

### Phase 6: expansion

Responsable principal: toi.

Objectif:

- remplacer `$VAR`
- remplacer `$?`
- ne pas expand dans les single quotes
- expand dans les double quotes
- gerer une variable inexistante comme une chaine vide

Tests:

```sh
echo $HOME
echo "$HOME"
echo '$HOME'
echo $?
echo $VARIABLE_QUI_N_EXISTE_PAS
```

### Phase 7: exec simple et builtins simples

Responsable principal: Dounia.

Objectif:

- executer une commande sans pipe ni redirection
- trouver les executables via `PATH`
- gerer `/bin/ls` et `./programme`
- coder `echo`, `pwd`, `env`
- recuperer l'exit status

Tests:

```sh
pwd
env
echo hello
ls
/bin/ls
```

### Phase 8: builtins qui modifient le shell

Responsable principal: Dounia.

Objectif:

- coder `cd`
- coder `export`
- coder `unset`
- coder `exit`
- executer ces builtins dans le parent quand ils sont seuls

Tests:

```sh
pwd
cd ..
pwd
export A=hello
env
unset A
exit
```

### Phase 9: redirections

Parsing: toi. Execution systeme: Dounia.

Objectif parsing:

- redirections stockees dans `cmd->redirs`
- le fichier cible ne doit pas etre dans `argv`
- erreur si une redirection n'a pas de cible

Objectif execution:

- ouvrir les fichiers avec les bons flags
- appliquer `dup2`
- fermer les fd inutiles

Flags utiles:

```text
>  : O_WRONLY | O_CREAT | O_TRUNC
>> : O_WRONLY | O_CREAT | O_APPEND
<  : O_RDONLY
```

Tests:

```sh
echo hello > out
cat < out
echo second >> out
cat out
echo >
cat <
```

### Phase 10: pipes

Parsing: toi. Execution systeme: Dounia.

Objectif parsing:

- chaque pipe cree une nouvelle `t_cmd`
- erreur si pipe au debut ou a la fin
- erreur si deux pipes se suivent

Objectif execution:

- creer les pipes
- fork chaque commande
- connecter stdin/stdout avec `dup2`
- attendre tous les enfants
- garder le bon exit status final

Tests:

```sh
ls | wc -l
env | grep PATH
cat file | grep hello | wc -l
|
echo hello |
echo hello || wc
```

Note: `||` n'est pas un bonus a faire au debut. Pour le obligatoire, ca doit
au moins etre refuse proprement ou traite comme erreur de syntaxe selon votre
choix de parsing.

### Phase 11: heredoc

Parsing: toi pour reconnaitre `<<` et le delimiter. Dounia pour lire le contenu
et le brancher sur l'entree de la commande.

Objectif:

- parser `<< delimiter`
- stocker le delimiter dans une redirection `R_HEREDOC`
- lire jusqu'au delimiter
- gerer `Ctrl-C` proprement pendant heredoc

Tests:

```sh
cat << EOF
grep hello << END
cat << EOF | wc -l
```

### Phase 12: signaux, leaks, norm

Objectif:

- `Ctrl-C` au prompt affiche une nouvelle ligne et un nouveau prompt
- `Ctrl-D` quitte le shell
- `Ctrl-\` ne fait rien au prompt interactif
- comportement adapte quand une commande tourne
- pas de leaks dans votre code
- norminette OK
- README en anglais

## 5. Checklist parsing pret pour Dounia

Ton parsing est utilisable par Dounia quand:

- `lexer` sort une liste de tokens correcte
- les tokens sont affichables en debug
- `parser` sort une liste de `t_cmd` correcte
- les commandes sont affichables en debug
- `argv` ne contient pas les redirections
- les redirections sont dans `cmd->redirs`
- les pipes sont representes par `cmd->next`
- les quotes de syntaxe sont retirees
- `$VAR` et `$?` sont expand quand il faut
- les single quotes bloquent l'expansion
- les erreurs de syntaxe sont detectees avant exec
- `free_tokens` et `free_cmds` existent
- les structs sont documentees dans le `.h`

## 6. Tests parsing seuls

Commandes valides:

```sh
echo hello
echo "hello world"
echo '$HOME'
echo "$HOME"
echo hello"world"
echo $?
cat < input
echo hello > out
echo hello >> out
ls | wc -l
cat < input | grep hello > out
```

Erreurs de syntaxe:

```sh
|
echo hello |
echo >
cat <
echo hello >>
echo "unclosed
echo 'unclosed
```

Pour ces tests, le but est d'afficher les tokens ou les `t_cmd`, pas encore de
lancer les commandes.

## 7. Tests execution seuls

Ces tests supposent que Dounia peut construire une `t_cmd` a la main ou utiliser
un parsing deja simple.

```sh
pwd
env
echo hello
ls
/bin/ls
cd ..
export A=hello
unset A
echo hello > out
cat < out
ls | wc -l
```

## 8. Tests integration ensemble

Quand parsing et execution sont branches ensemble:

```sh
pwd
cd ..
echo hello
echo $HOME
echo "$HOME"
echo '$HOME'
echo $?
env | grep PATH
ls | wc -l
echo hello > file
cat < file
cat << EOF
```

Comparer avec `bash` pour les cas ou vous hésitez.

## 9. Coordination Git et travail d'equipe

Objectif: pouvoir travailler chacune de son cote sans casser le travail de
l'autre, et garder une version stable du projet.

### Branches conseillees

```text
main         version stable, qui compile
chloe        travail parsing de Chloe
dounia       travail execution de Dounia
integration  branche pour tester vos deux parties ensemble
```

Regle simple:

- `main` doit rester propre
- Chloe travaille sur `chloe`
- Dounia travaille sur `dounia`
- vous mergez dans `integration` pour tester ensemble
- vous mergez `integration` dans `main` seulement quand ca compile

### Routine de base avant de coder

Au debut d'une session:

```sh
git status
git checkout chloe
git pull origin main
```

Pour Dounia:

```sh
git status
git checkout dounia
git pull origin main
```

Si une branche n'existe pas encore:

```sh
git checkout -b chloe
git push -u origin chloe
```

ou:

```sh
git checkout -b dounia
git push -u origin dounia
```

### Routine pendant le travail

Faire des petits commits, pas un enorme commit a la fin de la semaine.

Exemples de bons commits:

```text
add token creation helpers
add simple lexer debug print
add exec path lookup
fix redirection open flags
```

Commandes:

```sh
git status
git add fichier1.c fichier2.h
git commit -m "add token creation helpers"
git push
```

Avant de commit:

- relire `git status`
- ne pas ajouter les fichiers generes par `make`
- verifier que le commit ne contient que le travail voulu
- faire `make` si le code est cense compiler

### Comment eviter les conflits

Regles simples:

- ne pas modifier le fichier de l'autre sans prevenir
- Chloe evite de toucher a `srcs/exec/` et `srcs/builtins/`
- Dounia evite de toucher a `srcs/parsing/`
- `include/minishell.h` est un fichier commun: en parler avant de changer les structs
- `Makefile` est commun: en parler avant gros changement
- faire des commits courts et frequents
- push souvent pour que l'autre voie l'avancement

Fichiers sensibles:

```text
include/minishell.h
Makefile
srcs/main.c
```

Ces fichiers peuvent creer des conflits parce que vous allez toutes les deux
en avoir besoin. Il faut les modifier avec prudence.

### Integration de vos branches

Quand Chloe a une partie parsing qui compile:

```sh
git checkout integration
git pull origin integration
git merge chloe
make
git push
```

Quand Dounia a une partie exec qui compile:

```sh
git checkout integration
git pull origin integration
git merge dounia
make
git push
```

Si `integration` compile et que les tests de base passent:

```sh
git checkout main
git pull origin main
git merge integration
make
git push
```

### Si conflit pendant un merge

Ne pas paniquer. Faire:

```sh
git status
```

Git va montrer les fichiers en conflit. Ouvrir ces fichiers, chercher les zones:

```text
<<<<<<<
=======
>>>>>>>
```

Puis:

- garder la bonne version du code
- supprimer les marqueurs `<<<<<<<`, `=======`, `>>>>>>>`
- relancer `make`
- finir le merge:

```sh
git add fichier_en_conflit.c
git commit
```

Si le conflit concerne `include/minishell.h`, le regler ensemble, parce que c'est
le contrat entre parsing et execution.

### Quand utiliser `main`

`main` ne sert pas a tester des essais rapides. `main` sert a garder une version
que vous pourriez rendre si besoin.

Avant de merge dans `main`, verifier:

- `make` passe
- pas de fichiers generes dans `git status`
- les tests de base passent
- Chloe et Dounia savent ce qui est merge

Workflow resume:

```text
1. Chloe code sur chloe
2. Dounia code sur dounia
3. chacune push regulierement
4. merge vers integration pour tester ensemble
5. merge vers main seulement si integration compile
```

## 10. Priorite ultra courte

Ordre a garder en tete:

```text
1. squelette + Makefile
2. structs communes
3. readline loop
4. lexer simple
5. parser simple
6. quotes
7. expansion
8. exec simple
9. builtins
10. redirections
11. pipes
12. heredoc
13. signals
14. leaks / norm / README
```

Ne commencez pas par heredoc, signaux ou bonus. Faites d'abord une version
minuscule qui marche, puis ajoutez une difficulte a la fois.
