# Plan d'attaque — reprise actuelle

Dernière mise à jour : 8 septembre 2026.

Ce fichier garde le nom historique, mais il sert maintenant de plan de reprise.

## Où on en est

La base de ma partie est bien avancée :

```text
lexer mandatory OK
expansion mandatory principale OK
remove quotes OK
syntax errors avec message/status OK
parser vers t_cmd OK
pipes vers plusieurs t_cmd OK
redirections classiques ouvertes dans fd_in/fd_out OK
```

La PR clean `chloe-clean-pr-v2` est mergée dans `origin/dev`.

`chloe` reste ma branche atelier :

```text
docs
tests temporaires
notes de travail
expérimentations
```

## Ce qui est solide

### Lexer

```text
T_WORD
T_PIPE
T_REDIR_IN
T_REDIR_OUT
T_APPEND
T_HEREDOC
quotes conservées
had_quotes gardé
prev/next dans la liste
quote non fermée bloque au tokenizer
```

### Expansion

```text
$VAR
$?
$digit
variables absentes
single/double quotes
variables collées
delimiter heredoc non expandé
```

### Parser / syntax

```text
validate_syntax bloque les pipes/redirections invalides
syntax error -> stderr + status 2
parse_tokens crée une liste de t_cmd
cmd_and_args ne contient pas les redirections
is_builtin est rempli
< > >> ouvrent fd_in/fd_out
open fail -> perror + status 1 + parse_tokens NULL
```

## Ce qu'il faut faire maintenant

Priorité sans heredoc :

```text
1. tester avec le vrai main et la vraie exec de Dounia
2. confirmer que syntax error ne lance pas parse/exec dans le vrai main
3. confirmer que open fail ne lance pas exec
4. tester redirections sur external simple
5. tester redirections sur builtin seul
6. tester redirections dans pipeline
7. corriger le bug get_env_value avec Dounia
8. norme + cleanup
```

Gros morceau restant mandatory :

```text
heredoc
```

## Tests manuels à faire avec le vrai minishell

Quand la boucle/exec de Dounia est assez branchée :

```sh
echo hello
echo "hello world"
echo '$USER'
echo "$USER"
echo $?
echo $MISSING
echo hi | wc -c
cat < infile
cat < missing
echo hi > out
echo hi >> out
echo hi > a > b
cat < infile | grep h > out
pwd > out
env > out
| echo
echo |
echo > |
```

À surveiller :

```text
messages sur stderr
status cohérent
pas d'exec après syntax error
pas d'exec après parse_tokens NULL
pas de fd leak évident
```

## Heredoc plus tard

Ce qui existe déjà :

```text
T_HEREDOC
delimiter non expandé
quotes retirées sur delimiter
had_quotes gardé
```

À faire :

```text
lire les lignes heredoc
créer un fd heredoc
expander le contenu si delimiter non quoté
ne pas expander si delimiter quoté
gérer Ctrl-C
cleanup
```

## Rappel Git

Pour les PR :

```text
ne pas PR depuis chloe
faire une branche clean depuis origin/dev
prendre seulement les fichiers code
vérifier make fclean && make
vérifier Files changed sur GitHub
```
