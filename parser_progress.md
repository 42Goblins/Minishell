# Avancement parser — 1 septembre 2026

## Objectif

Le parser doit transformer les tokens déjà préparés en structures utilisables par
l'exec.

Pipeline actuelle :

```text
tokenizer
-> expand_tokens
-> remove_quotes_from_tokens
-> validate_syntax
-> parse_tokens
-> t_cmd / cmd_and_args
```

Le parser ne doit pas refaire le lexer, l'expansion ou le retrait des quotes.
Il reçoit des tokens déjà propres.

## Fichiers concernés

```text
srcs/parser/parser.c
srcs/parser/syntax.c
srcs/parser/parser_utils.c
tests/test_parser.c
tests/test_syntax.c
tests/test_loop.c
```

Prototypes dans :

```text
include/minishell.h
```

## État actuel

Déjà en place :

```text
parse_tokens
create_cmd_node
create_cmd_args
count_cmd_args
validate_syntax
is_redirection_token
```

Ce qui marche maintenant :

```text
parse_tokens crée une t_cmd pour une commande simple.
cmd_and_args est dupliqué avec ft_strdup.
Les redirections et leur filename/delimiter sont ignorés dans cmd_and_args.
validate_syntax refuse les erreurs simples avant parse_tokens.
tests/test_loop.c teste la pipeline locale jusqu'au parser.
```

Exemples attendus :

```text
echo hello        -> ["echo", "hello", NULL]
echo "hi there"   -> ["echo", "hi there", NULL]
echo '$USER'      -> ["echo", "$USER", NULL]
echo hi > out     -> ["echo", "hi", NULL]
cat < infile      -> ["cat", NULL]
echo hi >> log    -> ["echo", "hi", NULL]
cat << EOF        -> ["cat", NULL]
```

## Syntaxe déjà validée

`validate_syntax` accepte :

```text
echo hello
echo hello | wc
cat < infile
echo hi > outfile
cat << EOF
echo hi >> outfile
```

`validate_syntax` refuse :

```text
|
| echo
echo |
echo || wc
echo >
echo <
echo >>
echo <<
echo > |
echo < >
```

Convention actuelle :

```text
validate_syntax retourne 0 si OK.
validate_syntax retourne 1 si erreur.
Elle ne modifie pas les tokens.
```

À brancher plus tard dans la vraie boucle :

```text
syntax error -> message sur stderr
syntax error -> *get_status() = 2
ne pas appeler parse_tokens ni exec si validate_syntax échoue
```

## Tests

Test syntax :

```sh
make -C libft
cc -Wall -Wextra -Werror \
-Iinclude -Ilibft/inc \
tests/test_syntax.c \
srcs/parser/syntax.c \
srcs/parser/parser_utils.c \
srcs/lexer/lexer.c \
srcs/lexer/lexer_nodes.c \
srcs/lexer/lexer_redir.c \
srcs/lexer/lexer_quotes.c \
srcs/lexer/lexer_utils.c \
libft/libft.a \
-o /tmp/test_syntax

/tmp/test_syntax
```

Dernier état connu :

```text
test_syntax : tous les cas PASS
```

Test parser :

```sh
make -C libft
cc -Wall -Wextra -Werror \
-Iinclude -Ilibft/inc \
tests/test_parser.c \
srcs/parser/parser.c \
srcs/parser/parser_utils.c \
srcs/parser/syntax.c \
srcs/expansion/expansion.c \
srcs/expansion/expand_tokens.c \
srcs/expansion/expansion_vars.c \
srcs/expansion/expansion_utils.c \
srcs/lexer/lexer.c \
srcs/lexer/lexer_nodes.c \
srcs/lexer/lexer_redir.c \
srcs/lexer/lexer_quotes.c \
srcs/lexer/lexer_utils.c \
srcs/builtins/cd.c \
srcs/exec/exec_external.c \
libft/libft.a \
-o /tmp/test_parser

/tmp/test_parser
```

Dernier état connu :

```text
tests redirections : PASS
tests $USER / $MISSING : FAIL à cause du bug connu dans get_env_value
```

## Reste à faire

### 1. Parser les pipes

Objectif :

```text
echo hello | wc -c
```

Résultat attendu :

```text
cmd 1: ["echo", "hello", NULL]
cmd 2: ["wc", "-c", NULL]
cmd1->next = cmd2
cmd2->next = NULL
```

À faire :

```text
faire parcourir parse_tokens sur toute la liste
créer une t_cmd à chaque début de commande
s'arrêter au pipe pour une commande
reprendre après le pipe pour la commande suivante
relier les t_cmd avec next
free proprement toute la liste de commandes si malloc échoue
```

### 2. Décider le contrat redirections

Point à voir avec Dounia :

```text
est-ce que le parser ouvre les fichiers et remplit fd_in / fd_out ?
ou est-ce que le parser stocke seulement type + filename et l'exec ouvre ?
```

Tant que ce n'est pas décidé, le comportement actuel est volontaire :

```text
les redirections ne vont pas dans cmd_and_args
mais elles ne sont pas encore stockées dans t_cmd
```

### 3. Gérer les redirections dans t_cmd

À faire après décision :

```text
redir input <
redir output >
append >>
heredoc << à part
plusieurs redirections dans la même commande
erreur si open fail
```

Exemples importants :

```text
echo hi > out
cat < infile
echo hi > a > b
cat < missing
echo hi >> log
```

### 4. Heredoc

Le lexer/expansion gardent déjà les infos nécessaires :

```text
token après T_HEREDOC non expandé
quotes retirées après expansion
had_quotes conservé sur le delimiter
```

À faire avec Dounia :

```text
qui lit le heredoc
qui expand le contenu du heredoc
comment transmettre had_quotes à l'exec
comment gérer Ctrl-C dans heredoc
```

### 5. Erreurs et status

À brancher dans la vraie boucle :

```text
messages sur stderr
syntax error -> *get_status() = 2
malloc fail -> cleanup propre
la boucle continue après une syntax error
```

### 6. Nettoyage final

Avant une PR clean :

```text
vérifier la norme
vérifier les prototypes dans minishell.h
ajouter/free la liste complète de t_cmd
relancer test_syntax
relancer test_parser
ne pas mettre les .md perso ni tests temporaires dans dev
```
