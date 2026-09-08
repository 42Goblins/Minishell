# Pipeline globale minishell — plan de travail

Dernière mise à jour : 8 septembre 2026.

## But

Avoir une séparation claire :

```text
ma partie prépare la commande
Dounia exécute une commande déjà préparée
```

L'exec ne doit pas refaire le lexer, l'expansion, le retrait des quotes ou le
parsing.

## Pipeline cible

```text
1. readline
2. tokenizer
3. expand_tokens
4. remove_quotes_from_tokens
5. validate_syntax
6. parse_tokens
7. launch_exec
8. cleanup de la commande courante
```

Règles :

```text
si tokenizer échoue -> pas d'expansion/parser/exec
si validate_syntax échoue -> pas de parser/exec
si parse_tokens échoue -> pas d'exec
```

## État Git

```text
origin/dev contient la PR clean chloe-clean-pr-v2.
chloe reste ma branche atelier avec docs et tests temporaires.
Pour une future PR, créer une branche clean depuis origin/dev.
```

## Déjà fait côté préparation commande

```text
lexer mandatory
quotes conservées dans token->value
had_quotes sur les tokens concernés
liste token doublement chaînée avec prev/next
expansion $VAR / $? / $digit
remove_quotes_from_tokens après expansion
validation syntaxique avec message + status 2
parser vers plusieurs t_cmd chaînés
cmd_and_args sans redirections
redirections classiques ouvertes dans fd_in/fd_out
open fail -> status 1 et parse_tokens NULL
free_cmds cleanup cmd_and_args/path/fd
```

## Tests locaux

`tests/test_loop.c` sert à tester la pipeline sans toucher au vrai `main` :

```text
readline
-> tokenizer
-> expand_tokens
-> remove_quotes_from_tokens
-> validate_syntax
-> parse_tokens
-> print tokens + cmd_and_args
```

Ce test ne lance pas l'exec. Il sert à vérifier que les données préparées sont
cohérentes avant de les donner à Dounia.

Dernier état :

```text
syntax error bloque parse_tokens
status syntax = 2
parser error/open fail bloque la commande
pipe dans quotes reste dans le mot
quote non fermée bloque au tokenizer
```

## Ce qui reste sans heredoc

Ordre le plus utile maintenant :

```text
1. tester la pipeline avec le vrai main de Dounia
2. vérifier que le vrai main ne lance pas exec après validate_syntax fail
3. vérifier que le vrai main ne lance pas exec après parse_tokens NULL
4. vérifier redirections avec external simple
5. vérifier redirections avec builtin seul
6. vérifier redirections dans pipelines
7. norme et cleanup final
```

Cas manuels à tester quand l'exec sera prête :

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
| echo
echo |
echo > |
```

## Heredoc

Le heredoc reste le gros morceau mandatory à faire avec Dounia.

Déjà prêt :

```text
token T_HEREDOC
delimiter non expandé par expand_tokens
quotes du delimiter retirées ensuite
had_quotes conservé
```

À faire :

```text
lire le contenu du heredoc
créer un fd utilisable comme fd_in
expander le contenu seulement si delimiter non quoté
gérer Ctrl-C pendant heredoc
cleanup fd/temp si erreur
```

## Points ouverts avec Dounia

```text
corriger get_env_value / set_env_value avec ft_strcmp(...) == 0
confirmer fd_in/fd_out sur builtins seuls
confirmer fd_in/fd_out sur external simple
confirmer cleanup exact des fd dans parent/child
organiser heredoc
```

## Future PR clean

Méthode :

```text
1. garder chloe comme atelier
2. fetch origin
3. créer une branche depuis origin/dev
4. checkout seulement les fichiers code utiles depuis chloe
5. make fclean && make
6. vérifier Files changed : pas de .md, pas de tests temporaires
7. ouvrir PR vers dev
```
