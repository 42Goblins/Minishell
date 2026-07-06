# Minishell - comprendre les commandes

Ce fichier sert a comprendre ce qu'on demande a minishell.

On va rester simple : une commande, ce que ca veut dire, ce que tu testes, puis ce que
le parser doit comprendre.

## 0. Idee de base

Quand tu tapes ca :

```sh
echo hello
```

Le shell ne voit pas juste une phrase. Il doit comprendre :

```text
commande: echo
argument: hello
```

Donc dans ta tete, tu peux imaginer :

```text
["echo", "hello"]
```

Le premier mot est souvent le nom de la commande.
Les mots suivants sont souvent les arguments.

## 1. `echo`

### Ce que ca fait

`echo` affiche du texte dans le terminal.

### Test 1

```sh
echo hello
```

Tu devrais voir :

```text
hello
```

Le shell comprend :

```text
commande: echo
argument 1: hello
```

Donc pour le parser :

```text
["echo", "hello"]
```

### Test 2

```sh
echo hello world
```

Tu devrais voir :

```text
hello world
```

Le shell comprend :

```text
commande: echo
argument 1: hello
argument 2: world
```

Donc pour le parser :

```text
["echo", "hello", "world"]
```

### Test 3

```sh
echo -n hello
```

Tu devrais voir `hello`, mais sans retour a la ligne apres.

Ici `-n` est une option de `echo`.

Le shell comprend :

```text
commande: echo
argument 1: -n
argument 2: hello
```

Donc pour le parser :

```text
["echo", "-n", "hello"]
```

## 2. `pwd`

### Ce que ca fait

`pwd` veut dire "print working directory".

En francais : affiche le dossier dans lequel tu te trouves maintenant.

### Test

```sh
pwd
```

Tu devrais voir un chemin, par exemple :

```text
/home/cmauley/minishell_perso
```

Le shell comprend :

```text
commande: pwd
pas d'argument
```

Donc pour le parser :

```text
["pwd"]
```

## 3. `ls`

### Ce que ca fait

`ls` affiche les fichiers d'un dossier.

### Test 1

```sh
ls
```

Ca affiche les fichiers du dossier actuel.

Le shell comprend :

```text
commande: ls
pas d'argument
```

Donc pour le parser :

```text
["ls"]
```

### Test 2

```sh
ls -la
```

Ca affiche les fichiers avec plus de details.

`-la` est une option donnee a `ls`.

Le shell comprend :

```text
commande: ls
argument 1: -la
```

Donc pour le parser :

```text
["ls", "-la"]
```

### Test 3

```sh
ls /tmp
```

Ca affiche les fichiers du dossier `/tmp`.

Le shell comprend :

```text
commande: ls
argument 1: /tmp
```

Donc pour le parser :

```text
["ls", "/tmp"]
```

## 4. `cd`

### Ce que ca fait

`cd` veut dire "change directory".

En francais : change le dossier courant.

### Test

```sh
pwd
cd ..
pwd
```

Le premier `pwd` affiche le dossier actuel.
`cd ..` remonte dans le dossier parent.
Le deuxieme `pwd` montre que tu as change de place.

### Pourquoi c'est special

`cd` doit modifier le shell lui-meme.

Si `cd` etait lance comme un programme normal, il changerait le dossier d'un processus
separe, puis ce processus mourrait. Ton shell, lui, resterait au meme endroit.

C'est pour ca que `cd` est un builtin : minishell doit le coder lui-meme.

Le parser comprend quand meme simplement :

```text
cd ..
-> ["cd", ".."]
```

## 5. Variables comme `$HOME`

### Ce que ca fait

Une variable d'environnement est un nom qui contient une valeur.

Par exemple, `HOME` contient souvent ton dossier utilisateur.

Quand tu tapes `$HOME`, le shell remplace `$HOME` par sa valeur.

### Test 1

```sh
echo $HOME
```

Tu devrais voir un chemin, par exemple :

```text
/home/cmauley
```

Le shell fait deux etapes :

```text
avant expansion: ["echo", "$HOME"]
apres expansion: ["echo", "/home/cmauley"]
```

`expansion` veut dire : remplacer une variable par sa valeur.

### Test 2

```sh
echo $PATH
```

`PATH` contient les dossiers ou le shell cherche les programmes.

Par exemple, quand tu tapes :

```sh
ls
```

Le shell cherche `ls` dans les dossiers de `PATH`.

## 6. Quotes simples `'`

### Ce que ca fait

Les quotes simples protegent tout ce qui est dedans.

Dans des quotes simples, le shell ne remplace pas `$HOME`.

### Test 1

```sh
echo 'hello world'
```

Tu vois :

```text
hello world
```

Sans les quotes, `hello` et `world` seraient deux arguments.
Avec les quotes, c'est un seul argument.

Pour le parser :

```text
["echo", "hello world"]
```

### Test 2

```sh
echo '$HOME'
```

Tu vois :

```text
$HOME
```

Le shell ne remplace pas `$HOME`, parce que c'est entre quotes simples.

Pour le parser :

```text
["echo", "$HOME"]
```

## 7. Quotes doubles `"`

### Ce que ca fait

Les quotes doubles protegent les espaces, mais elles laissent quand meme les variables
s'expandre.

### Test 1

```sh
echo "hello world"
```

Tu vois :

```text
hello world
```

Pour le parser :

```text
["echo", "hello world"]
```

### Test 2

```sh
echo "$HOME"
```

Tu vois la valeur de `HOME`, par exemple :

```text
/home/cmauley
```

Difference importante :

```sh
echo '$HOME'
```

affiche le texte `$HOME`.

```sh
echo "$HOME"
```

affiche la valeur de `HOME`.

## 8. Redirection `>`

### Ce que ca fait

Normalement, une commande affiche dans le terminal.

Avec `>`, on dit : "au lieu d'afficher dans le terminal, ecris dans ce fichier".

### Test

```sh
echo hello > file
cat file
```

La premiere commande n'affiche rien dans le terminal.
Elle cree ou remplace le fichier `file`.

La deuxieme commande affiche le contenu du fichier.

Tu devrais voir :

```text
hello
```

Le shell comprend :

```text
commande: echo
argument: hello
redirection: sortie vers file
```

Pour le parser :

```text
args: ["echo", "hello"]
redirections: [">", "file"]
```

Point important : `>` et `file` ne sont pas des arguments de `echo`.
Ils servent au shell pour preparer l'execution.

## 9. Redirection `>>`

### Ce que ca fait

`>>` ajoute a la fin d'un fichier.

`>` remplace le fichier.
`>>` garde l'ancien contenu et ajoute apres.

### Test

```sh
echo one > file
echo two >> file
cat file
```

Tu devrais voir :

```text
one
two
```

Pour le parser :

```text
args: ["echo", "two"]
redirections: [">>", "file"]
```

## 10. Redirection `<`

### Ce que ca fait

Normalement, une commande lit depuis le clavier.

Avec `<`, on dit : "lis depuis ce fichier".

### Test

```sh
cat < file
```

`cat` lit le contenu de `file`, puis l'affiche.

Pour le parser :

```text
args: ["cat"]
redirections: ["<", "file"]
```

Encore une fois : `<` et `file` ne sont pas des arguments de `cat`.
Ils servent au shell.

## 11. Pipe `|`

### Ce que ca fait

Un pipe connecte deux commandes.

La sortie de la commande de gauche devient l'entree de la commande de droite.

### Test

```sh
ls | wc -l
```

Lecture humaine :

```text
ls produit une liste de fichiers
wc -l compte le nombre de lignes recues
```

Donc `wc -l` compte le nombre de lignes produites par `ls`.

Le shell comprend deux commandes :

```text
commande 1: ["ls"]
commande 2: ["wc", "-l"]
```

Le `|` ne fait partie d'aucune des deux commandes.
Il sert a les connecter.

## 12. `$?`

### Ce que ca fait

`$?` contient le code de retour de la derniere commande.

Une commande qui reussit retourne souvent `0`.
Une commande qui echoue retourne souvent autre chose que `0`.

### Test 1

```sh
pwd
echo $?
```

`pwd` reussit normalement.
Tu devrais voir :

```text
0
```

### Test 2

```sh
ls fichier_qui_n_existe_pas
echo $?
```

`ls` echoue parce que le fichier n'existe pas.
Tu devrais voir un nombre different de `0`.

Pour le parser :

```text
echo $?
-> remplacer $? par le dernier code de retour
```

## 13. `cat`

### Ce que ca fait

`cat` affiche le contenu d'un fichier.

### Test

```sh
echo hello > file
cat file
```

Tu devrais voir :

```text
hello
```

Le shell comprend :

```text
cat file
-> ["cat", "file"]
```

Ici `file` est un argument de `cat`.

Compare avec :

```sh
cat < file
```

Ici `file` est utilise par la redirection `<`.

Resultat visible souvent pareil, mais parsing different :

```text
cat file
-> args: ["cat", "file"]
-> redirections: []
```

```text
cat < file
-> args: ["cat"]
-> redirections: ["<", "file"]
```

## 14. Ce qu'il faut observer aujourd'hui

Ne fais pas tout d'un coup. Fais juste cette mini-liste :

```sh
echo hello
echo hello world
echo -n hello
pwd
ls
ls -la
cd ..
pwd
echo $HOME
echo '$HOME'
echo "$HOME"
echo hello > file
cat file
echo two >> file
cat file
cat < file
ls | wc -l
pwd
echo $?
ls fichier_qui_n_existe_pas
echo $?
```

## 15. Phrase a retenir

Le parsing sert a transformer une ligne de texte en informations rangees.

Exemple :

```sh
echo hello > file
```

devient :

```text
commande: echo
arguments: hello
redirection: ecrire la sortie dans file
```

Et seulement apres, la partie execution utilise ces informations pour vraiment faire le
travail.
