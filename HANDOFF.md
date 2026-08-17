# HANDOFF — Minishell

Dernière mise à jour : 17 août 2026, branche `chloe`.

## Contexte

Projet 42 minishell, mandatory uniquement.

Répartition actuelle :

```text
Chloé  : lexer, tokenisation, expansion, retrait des quotes, syntaxe, parser.
Dounia : env, builtins, exec, fd, heredoc, signaux, status.
```

Référence locale : `/home/gpalemo/minishell_nico`.

Nico met l'expansion dans `src/env`, avec une logique proche :

```text
dupliquer la string
scanner
remplacer les variables dans la string
reprendre le scan après la valeur insérée
```

Nous gardons cette idée, sans copier son code.

## Pipeline proposée

```text
readline
-> tokenizer
-> expand_tokens
-> remove_quotes_from_tokens
-> syntax validation
-> parser vers t_cmd / cmd_and_args
-> exec
-> cleanup commande courante
```

Ne pas appeler `remove_quotes_from_tokens` avant `expand_tokens`.

## Lexer

État : V1 mandatory considérée bonne.

Fichiers :

```text
srcs/lexer/lexer.c
srcs/lexer/lexer_utils.c
srcs/lexer/lexer_nodes.c
srcs/lexer/lexer_redir.c
srcs/lexer/lexer_quotes.c
```

Tokens :

```text
T_WORD
T_PIPE
T_REDIR_IN
T_REDIR_OUT
T_APPEND
T_HEREDOC
```

La liste est doublement chaînée :

```text
token->prev
token->next
```

`prev` sert notamment à reconnaître le delimiter heredoc dans `expand_tokens`.

`had_quotes` reste vrai même après retrait des quotes, parce que le heredoc a
besoin de savoir si son delimiter était quoté.

## Expansion

État : cœur de l'expansion terminé et testé en isolation + pipeline simple.

Fichiers :

```text
srcs/expansion/expansion.c
srcs/expansion/expand_tokens.c
srcs/expansion/expansion_vars.c
srcs/expansion/expansion_utils.c
srcs/utils/get_status.c
tests/test_expansion.c
```

Cas gérés :

```text
$VAR
$?
$digit
variables absentes
single quotes : pas d'expansion
double quotes : expansion
variables collées
```

Cas volontairement non gérés pour l'instant :

```text
$$
$-
${VAR}
```

`expand_tokens` :

```text
expand les T_WORD
skip le T_WORD juste après T_HEREDOC
```

Le delimiter heredoc n'est pas expandé, mais il passe ensuite dans
`remove_quotes_from_tokens`.

## Tests

Commande expansion :

```sh
make -C libft
cc -Wall -Wextra -Werror \
-Iinclude -Ilibft/inc \
tests/test_expansion.c \
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
srcs/utils/get_status.c \
libft/libft.a \
-o /tmp/test_expansion

/tmp/test_expansion
```

Dernier état connu : compilation OK, tests expansion OK.

## Contrat avec Dounia

Voir `contract_avec_dounia.md`.

Points à valider ensemble :

- structure exacte attendue par l'exec ;
- format de `t_cmd` / `cmd_and_args` ;
- gestion des redirections ;
- qui ouvre les fichiers ;
- qui lit le heredoc ;
- où écrire `*get_status()`.

## Prochaine étape

Si Dounia est avec Chloé :

```text
valider le contrat parser -> exec
```

Questions concrètes :

```text
Est-ce que l'exec veut cmd_and_args déjà prêt ?
Où stocker les redirections ?
Qui ouvre les fd ?
Qui libère les structures ?
```

Si Chloé avance seule :

```text
commencer le parser minimal commande simple -> cmd_and_args
```

Exemple cible :

```sh
echo "$USER" '$USER'
```

Après pipeline :

```text
tokens : echo, chloe, $USER
cmd_and_args : ["echo", "chloe", "$USER", NULL]
```

Ne pas brancher définitivement dans le main loop sans accord avec Dounia si son
exec est en mouvement.
