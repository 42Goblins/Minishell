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
srcs/parser/parser_redir.c
srcs/parser/syntax.c
srcs/parser/parser_utils.c
srcs/utils/free_cmds.c
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
create_cmd_and_args
count_cmd_args
validate_syntax
is_redirection_token
free_cmds
open_redirections
open_current_redirection
open_input_redirection
open_output_redirection
```

Ce qui marche maintenant :

```text
parse_tokens crée une liste de t_cmd pour les commandes séparées par des pipes.
cmd_and_args est dupliqué avec ft_strdup.
Les redirections et leur filename/delimiter sont ignorés dans cmd_and_args.
Les redirections classiques ouvrent fd_in ou fd_out dans t_cmd.
validate_syntax refuse les erreurs simples avant parse_tokens.
tests/test_loop.c teste la pipeline locale jusqu'au parser.
free_cmds libère la liste de commandes déjà créée en cas d'erreur.
free_cmds ferme aussi les fd de redirection encore ouverts au cleanup.
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
echo hi | wc -c   -> cmd1 ["echo", "hi", NULL], cmd2 ["wc", "-c", NULL]
cat < in          -> cmd->fd_in ouvert
echo hi > out     -> cmd->fd_out ouvert
echo hi >> log    -> cmd->fd_out ouvert en append
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
srcs/parser/parser_redir.c \
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
srcs/utils/free_cmds.c \
libft/libft.a \
-o /tmp/test_parser

/tmp/test_parser
```

Dernier état connu :

```text
tests redirections : PASS
tests pipes : PASS
tests fd redirections : PASS
tests $USER / $MISSING : FAIL à cause du bug connu dans get_env_value
```

## Reste à faire

### 1. Parser les pipes

État : V1 faite et testée.

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

Déjà fait :

```text
parse_tokens parcourt maintenant toute la liste de tokens
create_cmd_node est appelé à chaque début de commande
le scan s'arrête au pipe pour une commande
parse_tokens reprend après le pipe pour la commande suivante
les t_cmd sont reliées avec next
free_cmds est appelé si une création de commande échoue
```

Tests ajoutés :

```text
echo hello | wc -c
echo hello | grep h | wc -l
cat < infile | grep hello > outfile
```

### 2. Redirections classiques

État : décision prise avec Dounia, V1 commencée et testée.

```text
ma partie ouvre les redirections classiques
ma partie remplit cmd->fd_in / cmd->fd_out
l'exec utilise les fd déjà prêts
```

Déjà fait :

```text
<  ouvre le fichier en O_RDONLY et remplit fd_in
>  ouvre le fichier en O_WRONLY | O_CREAT | O_TRUNC et remplit fd_out
>> ouvre le fichier en O_WRONLY | O_CREAT | O_APPEND et remplit fd_out
si plusieurs redirections du même côté existent, la dernière gagne
l'ancien fd est fermé avant d'être remplacé
open fail affiche une erreur avec perror
open fail met *get_status() = 1
open fail fait échouer parse_tokens
```

Tests ajoutés :

```text
cat < /tmp/minishell_parser_infile
echo hi > /tmp/minishell_parser_out
echo hi >> /tmp/minishell_parser_log
echo hi > /tmp/minishell_parser_a > /tmp/minishell_parser_b
cat < /tmp/minishell_missing_input
cat < infile | grep hello > outfile
```

À garder en tête :

```text
free_cmds ferme les fd encore stockés dans t_cmd.
Dans un pipeline, un close dans le child après dup2 ne ferme pas le fd du parent.
Si l'exec ferme un fd dans le process parent, il doit remettre fd_in/fd_out à 0/1.
```

### 3. Redirections : reste à solidifier

```text
tester fd_in + fd_out dans la même commande
tester plusieurs redirections input : cat < a < b
tester redirection dans chaque segment d'un pipe
brancher proprement l'erreur redirection dans la vraie boucle
vérifier que l'exec ne lance pas la commande si parse_tokens retourne NULL
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
vérifier free_cmds avec les futurs fd_in/fd_out
relancer test_syntax
relancer test_parser
ne pas mettre les .md perso ni tests temporaires dans dev
```
