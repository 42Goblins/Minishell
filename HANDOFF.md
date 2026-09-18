# HANDOFF — Minishell

Dernière mise à jour : 8 septembre 2026, branche `chloe`.

## Situation Git

```text
origin/dev contient la PR clean chloe-clean-pr-v2.
chloe reste ma branche atelier avec tests temporaires et docs.
Ne pas ouvrir une PR directement depuis chloe.
```

La PR clean récente a ajouté au `dev` :

```text
parser_redir.c
parser_utils.c
syntax.c
free_cmds.c
prototypes nécessaires dans minishell.h
petites mises à jour lexer/expansion
```

## Répartition

```text
Moi    : lexer, tokenisation, expansion, retrait des quotes, syntaxe, parser.
Dounia : env, builtins, exec, wait/status, fd côté exec, heredoc, signaux.
```

Contrat : l'exec ne relit pas la ligne brute. Elle reçoit une liste de `t_cmd`
déjà préparée.

## Pipeline retenue

```text
readline
-> tokenizer
-> expand_tokens
-> remove_quotes_from_tokens
-> validate_syntax
-> parse_tokens
-> launch_exec
-> cleanup de la commande courante
```

Important :

```text
expand_tokens avant remove_quotes_from_tokens
validate_syntax avant parse_tokens
si validate_syntax échoue, ne pas appeler parse_tokens
si parse_tokens retourne NULL, ne pas appeler exec
```

## Lexer

État : considéré bon pour le mandatory.

Tokens :

```text
T_WORD
T_PIPE
T_REDIR_IN
T_REDIR_OUT
T_APPEND
T_HEREDOC
```

La liste est doublement chaînée avec `prev` et `next`.

Les quotes sont conservées jusqu'à l'expansion, puis retirées après. `had_quotes`
reste disponible après retrait des quotes, surtout pour heredoc.

## Expansion

État : cœur mandatory fait.

Géré :

```text
$VAR
$?
$digit
variable absente -> chaîne vide
single quotes -> pas d'expansion
double quotes -> expansion
variables collées
```

Pas géré volontairement pour l'instant :

```text
$$
$-
${VAR}
```

Attention bug connu côté env :

```text
get_env_value / set_env_value doivent comparer avec ft_strcmp(...) == 0
```

Tant que ce bug existe, `$USER`, `$HOME` ou `$MISSING` peuvent sortir faux dans
les tests d'intégration.

## Parser

État : parser/pipes/redirections classiques en place.

Fonctions principales :

```text
validate_syntax
parse_tokens
create_cmd_node
create_cmd_and_args
count_cmd_args
open_redirections
is_redirection_token
free_cmds
```

Ce qui marche :

```text
cmd_and_args ne contient pas les redirections ni leurs filenames
les pipes créent plusieurs t_cmd chaînés
is_builtin est rempli avec check_is_builtins
< ouvre fd_in
> ouvre fd_out truncate
>> ouvre fd_out append
open fail -> perror, status 1, parse_tokens NULL
syntax error -> stderr, status 2, pas de parse_tokens
```

Exemples :

```text
echo hi > out          -> ["echo", "hi", NULL], fd_out vers out
cat < infile           -> ["cat", NULL], fd_in depuis infile
echo hi | wc -c        -> deux t_cmd
cat text | "a | b"     -> le pipe dans quotes reste dans le WORD
```

## Tests locaux

Les tests locaux restent surtout pour bosser sur `chloe`.

Derniers résultats connus :

```text
test_syntax : PASS
test_parser : parser/pipes/redirs/open fail PASS
test_loop   : syntax error bloque parse_tokens, status 2
```

Fails connus dans `test_parser` :

```text
quotes + expansion
missing variable
```

Cause : bug env connu, pas parser.

## Commande test_loop

```sh
make -C libft
cc -Wall -Wextra -Werror \
-Iinclude -Ilibft/inc \
tests/test_loop.c \
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
srcs/parser/parser_utils.c \
srcs/parser/syntax.c \
srcs/env/setup_env.c \
srcs/builtins/cd.c \
srcs/exec/exec_external.c \
srcs/utils/free_cmds.c \
libft/libft.a \
-lreadline -ltermcap \
-o /tmp/test_loop

/tmp/test_loop
```

## Reste à faire

Sans heredoc, il reste surtout :

```text
tester avec le vrai main/exec de Dounia
vérifier que l'exec utilise fd_in/fd_out pour external simple
vérifier que les builtins seuls sauvent/restaurent les fd
vérifier redirections + pipelines dans le vrai minishell
faire norme/cleanup final
```

Heredoc reste le gros morceau obligatoire pas terminé :

```text
lecture heredoc
fd heredoc
expansion du contenu selon had_quotes
Ctrl-C heredoc
intégration avec exec
```
