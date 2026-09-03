# Plan d'attaque — vendredi 28 août

## Objectif de la journée

Avancer au maximum sur ma partie pour le mandatory, sans dépendre de
l'exec de Dounia.

Ta partie :

```text
lexer / tokenisation
expansion
retrait des quotes
validation syntaxique
parser vers t_cmd / cmd_and_args
préparation de la pipeline avant exec
```

Objectif concret initial :

```text
Avoir une pipeline locale solide :
readline -> tokenizer -> expand_tokens -> remove_quotes_from_tokens
-> parse_tokens -> affichage cmd_and_args
```

Si ça avance bien :

```text
commencer syntax validation
commencer parser redirections
commencer parser pipes
```

État au 1 septembre :

```text
Pipeline locale jusqu'à parse_tokens : faite dans tests/test_loop.c.
Syntax validation : faite + test_syntax.
Parser cmd_and_args : ignore maintenant les redirections et leur filename.
Prochain gros morceau : parser les pipes vers plusieurs t_cmd.
Ensuite : décider/faire le stockage ou l'ouverture des redirections.
```

Ne pas perdre du temps à brancher le vrai `main.c` si l'exec est encore WIP.
Pour demain, le terrain de jeu principal reste :

```text
tests/test_loop.c
tests/test_parser.c
srcs/parser/parser.c
```

## État actuel à retenir

La PR clean lexer / expansion / parser a été mergée dans `dev`.

La branche `chloe` reste ma branche atelier :

```text
.md perso
tests temporaires
expérimentations
```

Ne pas ouvrir de PR directement depuis `chloe` telle quelle.

Fichiers importants :

```text
srcs/lexer/...
srcs/expansion/...
srcs/parser/parser.c
srcs/parser/parser_redir.c
tests/test_loop.c
tests/test_parser.c
HANDOFF.md
pipeline_plan.md
contract_avec_dounia.md
```

## Ce qui est déjà plutôt OK

### Lexer / tokenizer

Le lexer est considéré V1 mandatory OK.

Il sait créer les tokens :

```text
T_WORD
T_PIPE
T_REDIR_IN
T_REDIR_OUT
T_APPEND
T_HEREDOC
```

Il conserve les quotes dans `token->value`.

Il pose `had_quotes = true` si le mot contenait des quotes.

Il relie la liste avec :

```text
prev
next
```

Le `prev` sert notamment à savoir si un mot est un delimiter de heredoc.

### Expansion

L'expansion gère déjà :

```text
$VAR
$?
$digit
variable absente -> string vide
pas d'expansion en single quotes
expansion en double quotes
variables collées
```

Cas volontairement pas gérés maintenant :

```text
$$
$-
${VAR}
```

Règle importante :

```text
expand_tokens avant remove_quotes_from_tokens
```

### Parser actuel

Le parser actuel sait faire une commande simple et ne met plus les filenames de
redirection dans `cmd_and_args`.

```text
tokens WORD -> t_cmd -> cmd_and_args
```

Exemple :

```text
echo hello -> ["echo", "hello", NULL]
echo hello > out -> ["echo", "hello", NULL]
cat << EOF -> ["cat", NULL]
```

Limite actuelle :

```text
redirections pas encore stockées/ouvertes dans t_cmd
pipes pas encore transformés en plusieurs t_cmd
pas encore de détection builtin
messages/status syntax pas encore centralisés
```

Exemple maintenant corrigé côté `cmd_and_args` :

```sh
echo hello > out
```

Le lexer tokenise bien :

```text
echo / hello / > / out
```

Le parser ne met plus `out` dans `cmd_and_args`. Plus tard il faudra encore
stocker/ouvrir la redirection :

```text
cmd_and_args = ["echo", "hello", NULL]
redir_out = out
```

## Point connu côté Dounia

Bug signalé :

```text
get_env_value / set_env_value doivent comparer avec ft_strcmp(...) == 0
```

Sinon `$USER` peut retourner une mauvaise variable d'environnement.

Conséquence pour demain :

```text
Si echo $USER sort une valeur bizarre, ne panique pas.
Ça ne veut pas forcément dire que ton expansion est cassée.
```

Tests fiables malgré ce bug :

```sh
echo hello
echo '$USER'
echo $?
echo $2USER
echo "hello world"
```

Tests dépendants du bug env :

```sh
echo $USER
echo "$HOME"
echo $PWD
```

## Programme de demain

### Bloc 0 — Reprendre proprement

But : retrouver un état local propre avant de coder.

Commandes :

```sh
git switch chloe
git status --short
make fclean
git status --short
```

À vérifier :

```text
Pas de fichiers compilés dans git status.
Si des .md sont modifiés, c'est probablement normal.
Si du code est modifié, relire avant de continuer.
```

Lire rapidement :

```text
HANDOFF.md
pipeline_plan.md
plan_attaque_28_aout.md
```

### Bloc 1 — Finir la mini boucle locale jusqu'aux quotes

Statut : fait.

Fichier :

```text
tests/test_loop.c
```

État actuel :

```text
readline
add_history
tokenizer
expand_tokens
print tokens
free tokens / line
```

Étape à faire :

```text
ajouter remove_quotes_from_tokens après expand_tokens
```

But :

```text
voir les tokens après expansion ET retrait des quotes
```

Exemples à tester :

```sh
echo "hello world"
echo '$USER'
echo "$USER"
echo '$USER'$HOME
cat << "EOF"
```

Ce que tu dois vérifier :

```text
"hello world" devient hello world
'$USER' devient $USER
"$USER" est expand avant que les quotes partent
le delimiter heredoc n'est pas expand par expand_tokens
had_quotes reste affiché à 1 si le token avait des quotes
```

Critère de fin du bloc :

```text
La mini boucle affiche des tokens propres après remove_quotes_from_tokens.
Pas de crash sur quotes simples/doubles.
Ctrl-D quitte proprement.
```

### Bloc 2 — Brancher parse_tokens dans test_loop

Statut : fait.

Fichier :

```text
tests/test_loop.c
```

Étape :

```text
après remove_quotes_from_tokens, appeler parse_tokens(shell.token)
mettre le retour dans shell.cmds
afficher cmd_and_args
free la commande
```

À faire avant ou pendant ce bloc :

```text
Créer un helper de test free_cmd si besoin.
Créer un helper print_cmd_args.
```

Ne pas appeler `launch_exec` encore.

But :

```text
valider readline -> lexer -> expansion -> quote removal -> parser
```

Tests à faire :

```sh
echo hello
echo "hello world"
echo '$USER'
echo $?
echo $2USER
```

Résultats attendus :

```text
echo hello          -> ["echo", "hello", NULL]
echo "hello world"  -> ["echo", "hello world", NULL]
echo '$USER'        -> ["echo", "$USER", NULL]
echo $?             -> ["echo", "0" ou status actuel, NULL]
echo $2USER         -> ["echo", "USER", NULL]
```

Critère de fin du bloc :

```text
La boucle interactive affiche cmd_and_args correctement pour les commandes simples.
```

### Bloc 3 — Consolider les tests parser

Statut : partiellement fait.

Déjà ajouté :

```text
tests redirections qui vérifient que les filenames ne vont plus dans cmd_and_args
test parser synchronisé avec validate_syntax
```

À garder en tête :

```text
les cas $USER / $MISSING échouent tant que get_env_value est faux côté env
```

Fichier :

```text
tests/test_parser.c
```

But :

```text
Avoir des tests non interactifs pour les cas déjà supportés.
```

Tests minimum à garder :

```text
echo hello
echo "hello world"
echo '$USER'
echo $?
echo $2USER $12USER
```

Ajouter si pas déjà fait :

```text
ligne vide ou tokens NULL -> parse_tokens retourne NULL
```

Critère de fin :

```text
test_parser compile
les cas simples passent
les résultats attendus sont affichés clairement
```

### Bloc 4 — Syntax validation V1

Statut : fait pour les erreurs simples robustes.

But : refuser les erreurs évidentes avant parser/exec.

Important : cette étape n'est pas jetable. `syntax.c` fait partie du vrai
minishell final. Ce qui peut rester temporaire, ce sont seulement les messages
debug dans `tests/test_loop.c`.

Nouveau fichier conseillé :

```text
srcs/parser/syntax.c
```

Nom de fonction possible :

```text
validate_syntax
```

Ce que la fonction doit vérifier en premier :

```text
pas de pipe au début
pas de pipe à la fin
pas de deux pipes d'affilée
pas de redirection sans filename après
pas de redirection suivie d'un autre opérateur invalide
```

Cas à refuser :

```sh
|
echo |
| echo
echo ||
echo >
echo <
echo >>
echo <<
echo > |
echo < >
```

Cas à accepter :

```sh
echo hello
echo hello | wc
cat < infile
echo hi > outfile
cat << EOF
echo hi >> outfile
```

Décision :

```text
La syntax validation retourne 0 si OK, 1 si erreur.
Elle ne modifie pas les tokens.
Elle affiche peut-être un message temporaire dans les tests.
```

Gestion d'erreur finale à prévoir :

```text
syntax error -> *get_status() = 2
message -> stderr
ne pas appeler parser
ne pas appeler exec
retourner à readline
```

Messages bash-like visés plus tard :

```text
minishell: syntax error near unexpected token `|'
minishell: syntax error near unexpected token `newline'
```

Pour avancer vite maintenant :

```text
1. validate_syntax détecte correctement et retourne 1
2. test_loop affiche temporairement "syntax error"
3. plus tard, centraliser les vrais messages et status avec Dounia
```

Critère de fin :

```text
La mini boucle refuse les erreurs de syntaxe avant parse_tokens.
```

État atteint :

```text
test_syntax.c ajouté
validate_syntax branché dans test_loop
validate_syntax branché dans test_parser
```

Si ce bloc devient trop long :

```text
faire seulement pipe errors + redirection sans filename
laisser les cas plus fins pour le lendemain
```

### Bloc 5 — Parser redirections V1

Statut : skip dans `cmd_and_args` fait.

But : arrêter de mettre les filenames de redirection dans `cmd_and_args`.

Avant de coder, décider avec le header actuel :

```text
t_cmd a fd_in et fd_out
mais pas encore de liste de redirections
```

Approche simple possible pour mandatory V1 :

```text
parser ouvre les fichiers et remplit fd_in / fd_out
cmd_and_args ignore les opérateurs et leurs filenames
```

Mais ce point touche Dounia.

Si pas validé avec elle :

```text
faire seulement le skip dans cmd_and_args
ne pas ouvrir les fichiers
documenter que fd_in/fd_out reste à brancher
```

Petite étape autonome safe :

```text
Modifier count_cmd_args pour ne pas compter le WORD qui suit une redirection.
Modifier create_cmd_and_args pour ne pas copier le WORD qui suit une redirection.
```

Exemple :

```sh
echo hello > out
```

Résultat parser attendu pour l'instant :

```text
cmd_and_args = ["echo", "hello", NULL]
```

Même si `out` n'est pas encore stocké quelque part.

Critère de fin :

```text
Les redirections ne polluent plus cmd_and_args.
```

État atteint :

```text
echo hi > out      -> ["echo", "hi", NULL]
cat < infile       -> ["cat", NULL]
echo hi >> log     -> ["echo", "hi", NULL]
cat << EOF         -> ["cat", NULL]
```

Reste à faire pour les redirections :

```text
décider avec Dounia si parser ouvre les fd ou stocke seulement les infos
puis remplir fd_in/fd_out ou une structure de redirections
```

### Bloc 6 — Parser pipes V1

But : transformer plusieurs commandes séparées par `|` en plusieurs `t_cmd`.

Exemple :

```sh
echo hello | wc -c
```

Résultat attendu :

```text
cmd 1: ["echo", "hello", NULL]
cmd 2: ["wc", "-c", NULL]
cmd1->next = cmd2
cmd2->next = NULL
```

Approche :

```text
parse_tokens devient vraiment utile
il parcourt les tokens
à chaque début de commande, create_cmd_node(current)
il saute jusqu'au prochain pipe
il relie les t_cmd avec next
```

Critère de fin :

```text
test_loop affiche plusieurs cmd_and_args quand il y a un pipe.
```

Ce bloc peut être gardé pour un autre jour si syntax + redirections prennent
déjà beaucoup de temps.

## Priorité réelle maintenant

Déjà fait :

```text
1. test_loop avec remove_quotes_from_tokens
2. test_loop avec parse_tokens + print cmd_and_args
3. tests parser propres pour commandes simples
4. syntax validation V1
5. skip redirections dans cmd_and_args
```

Prochaine priorité :

```text
1. parser pipes vers plusieurs t_cmd
2. tester cmd1/cmd2 dans test_parser ou test_loop
```

Ensuite :

```text
3. contrat redirections avec Dounia
4. stockage/ouverture redirections
```

Gros morceau partagé restant :

```text
5. heredoc complet
```

À ne pas faire demain sauf si tout le reste est vraiment propre :

```text
brancher définitivement dans main.c
refacto lourd expand_word
norminette complète de tout
gestion complète heredoc
exec finale
signaux
centralisation finale de tous les messages d'erreur
```

## Erreurs : qui gère quoi

Répartition pour ne pas se marcher dessus avec Dounia :

```text
Ma partie :
- quote non fermée
- erreurs de syntaxe avant parser
- parser impossible / malloc fail

Dounia :
- command not found
- permission denied
- erreurs builtins
- erreurs open/dup/fork/execve/waitpid
- signaux
```

Codes importants à garder en tête :

```text
syntax error        -> 2
command not found   -> 127
permission denied   -> 126
redirection open KO -> 1
Ctrl-C              -> 130
Ctrl-\              -> 131
```

Point à discuter avec Dounia :

```text
Est-ce que les fonctions de validation print/set status directement,
ou est-ce que la boucle principale centralise ça ?
```

Décision provisoire :

```text
Mes fonctions retournent 0/1.
Le branchement final des messages exacts et de *get_status() se fera quand la
boucle principale sera décidée.
```

## Commandes utiles

Compiler la mini boucle locale :

```sh
make -C libft
cc -Wall -Wextra -Werror \
-Iinclude -Ilibft/inc \
tests/test_loop.c \
srcs/env/setup_env.c \
srcs/lexer/lexer.c \
srcs/lexer/lexer_nodes.c \
srcs/lexer/lexer_redir.c \
srcs/lexer/lexer_quotes.c \
srcs/lexer/lexer_utils.c \
srcs/expansion/expansion.c \
srcs/expansion/expand_tokens.c \
srcs/expansion/expansion_vars.c \
srcs/expansion/expansion_utils.c \
srcs/parser/parser.c \
srcs/parser/parser_redir.c \
srcs/parser/syntax.c \
srcs/parser/parser_utils.c \
srcs/builtins/cd.c \
srcs/exec/exec_external.c \
libft/libft.a \
-lreadline -ltermcap \
-o /tmp/test_loop

/tmp/test_loop
```

Compiler tout le projet :

```sh
make fclean
make
```

Voir ce qui a changé :

```sh
git status --short
git diff --stat
```

Chercher un symbole :

```sh
rg -n "parse_tokens|create_cmd_and_args|remove_quotes_from_tokens|validate_syntax"
```

## Checklist de fin de journée

Avant d'arrêter :

```text
[ ] git status lu
[ ] make passe ou l'erreur restante est notée
[ ] tests locaux utiles lancés
[ ] .md mis à jour si une décision importante a changé
[ ] commit fait sur chloe
[ ] push origin chloe si tu changes d'ordi
```

Commande de commit atelier possible :

```sh
git add HANDOFF.md pipeline_plan.md contract_avec_dounia.md expansion_progress.md tokenizer_progress.md plan_attaque_28_aout.md tests/test_loop.c tests/test_parser.c srcs/parser
git commit -m "wip: advance local parser pipeline"
git push origin chloe
```

Avant une future PR vers `dev` :

```text
Ne pas PR depuis chloe directement.
Créer une branche clean depuis origin/dev.
Restaurer uniquement les fichiers de code.
Vérifier Files changed sur GitHub.
```

## Si tu bloques demain

Ordre de debug :

```text
1. Est-ce que tokenizer sort les bons tokens ?
2. Est-ce que expand_tokens change les bonnes values ?
3. Est-ce que remove_quotes_from_tokens nettoie les quotes ?
4. Est-ce que parse_tokens reçoit bien les tokens déjà nettoyés ?
5. Est-ce que cmd_and_args contient seulement les vrais arguments ?
```

Ne debug pas tout en même temps.

Toujours afficher l'état après chaque étape :

```text
après tokenizer
après expansion
après remove quotes
après parser
```

Le but de demain n'est pas d'avoir le minishell final. Le but est d'avoir une
pipeline de mon côté claire, testable, et assez solide pour être branchée avec
Dounia dès que l'exec est prêt.
