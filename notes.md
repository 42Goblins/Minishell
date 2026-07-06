# Minishell - notes de depart

Projet avec Dounia.

Objectif pour le debut : comprendre assez le sujet pour pouvoir coder petit a petit, sans essayer de tout avaler d'un coup.

## 1. Avant de coder

- Relire le sujet officiel une premiere fois sans prendre trop de notes.
- Refaire une deuxieme lecture en listant seulement :
  - les fonctions autorisees
  - ce qui est obligatoire
  - ce qui est interdit
  - les cas de syntaxe a gerer
- Tester un vrai shell comme `bash` pour observer son comportement.

Exemples a tester dans `bash` :

```sh
echo hello
echo "hello world"
echo 'hello world'
echo $HOME
pwd
cd ..
ls -la | wc -l
echo hi > file
cat < file
```

## 2. Grande idee du projet

Un minishell fait souvent cette chaine :

```text
ligne entree par l'utilisateur
-> parsing
-> structure de commande
-> execution
-> affichage / code de retour
```

Le parsing transforme du texte en informations claires.
L'execution utilise ces informations pour lancer les commandes.

## 3. Repartition possible

Moi : parsing

- comprendre les quotes `'` et `"`
- decouper la ligne en tokens
- reconnaitre les pipes `|`
- reconnaitre les redirections `<`, `>`, `>>`, `<<`
- gerer les variables comme `$HOME` et `$?`
- construire une structure utilisable par l'exec

Dounia : execution

- executer les builtins : `cd`, `echo`, `pwd`, `export`, `unset`, `env`, `exit`
- trouver les commandes avec `PATH`
- lancer les programmes avec `fork`, `execve`, `wait`
- connecter les pipes
- appliquer les redirections
- gerer les codes de retour

Point important : parsing et execution doivent se mettre d'accord tot sur une structure commune.

## 4. Premiere structure mentale

Une commande simple pourrait devenir :

```text
commande = {
  args: ["echo", "hello"],
  redirections: [],
}
```

Une ligne avec pipe pourrait devenir :

```text
pipeline = [
  { args: ["ls", "-la"], redirections: [] },
  { args: ["wc", "-l"], redirections: [] }
]
```

Pour le moment, ce n'est pas du vrai code obligatoire. C'est juste une facon de penser.

## 5. Plan tres progressif

### Etape 1 - Observer

- Lancer plein de petites commandes dans `bash`.
- Noter les comportements surprenants.
- Ne pas coder tout de suite les cas compliques.

### Etape 2 - Faire un mini parser simple

Objectif : gerer seulement des commandes sans quotes, sans pipes, sans redirections.

Exemples :

```sh
echo hello
pwd
ls -la
```

### Etape 3 - Ajouter les quotes

Comprendre la difference entre :

```sh
echo '$HOME'
echo "$HOME"
echo hello"world"
```

### Etape 4 - Ajouter pipes et redirections

Seulement apres avoir un parser simple qui marche.

### Etape 5 - Relier avec l'exec

Faire des petits tests ensemble avec Dounia pour verifier que la structure produite par le parser est pratique pour l'execution.

## 6. Questions a clarifier avec Dounia

- Quelle structure commune entre parsing et execution ?
- Qui gere l'expansion des variables ?
- Qui detecte les erreurs de syntaxe ?
- Comment tester chaque partie sans attendre que tout soit fini ?

## 7. Regle perso

Ne pas essayer de faire un shell complet d'un coup.
Faire une version minuscule qui marche, puis ajouter une difficulte a la fois.
