# HANDOFF — Minishell

Dernière mise à jour : 19 août 2026, branche `chloe`.

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

État d'intégration actuel :

```text
dev contient maintenant la PR clean lexer / expansion / parser.
chloe reste la branche atelier avec les .md et tests temporaires.
tests/test_loop.c teste une mini boucle readline locale.
```

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

État : cœur de l'expansion terminé et testé en isolation + début de boucle
interactive locale.

Fichiers :

```text
srcs/expansion/expansion.c
srcs/expansion/expand_tokens.c
srcs/expansion/expansion_vars.c
srcs/expansion/expansion_utils.c
tests/test_expansion.c
tests/test_loop.c
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

Attention : après merge avec `dev`, `get_status()` est actuellement défini côté
Dounia dans `srcs/main.c`. Les tests locaux peuvent utiliser un stub de
`get_status()` dans leur propre fichier pour éviter de compiler deux `main`.

Point signalé à Dounia : `get_env_value` / `set_env_value` doivent comparer les
clés avec `ft_strcmp(...) == 0`. Sinon `$USER` peut récupérer la mauvaise valeur
d'environnement.

## Parser

État : V1 commande simple en place.

Fichier :

```text
srcs/parser/parser.c
```

Fonctions actuelles :

```text
parse_tokens
create_cmd_node
create_cmd_args
count_cmd_args
```

Ce qui marche :

```text
tokens WORD préparés
-> t_cmd
-> cmd_and_args dupliqué
```

Exemple :

```text
echo hello -> ["echo", "hello", NULL]
```

Limites actuelles :

```text
pas encore de parsing complet des pipes
pas encore de redirections stockées dans t_cmd
pas encore de détection builtin
```

## Tests

Commande expansion historique :

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
libft/libft.a \
-o /tmp/test_expansion

/tmp/test_expansion
```

Note : depuis le merge avec `dev`, `get_status()` est dans `srcs/main.c`. Pour
compiler un test sans le vrai `main`, ajouter un petit stub `get_status()` dans
le fichier de test, comme dans `tests/test_loop.c`.

Mini boucle locale actuelle :

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
srcs/builtins/cd.c \
libft/libft.a \
-lreadline -ltermcap \
-o /tmp/test_loop

/tmp/test_loop
```

Ce test couvre actuellement :

```text
readline
-> add_history
-> tokenizer
-> expand_tokens
-> print tokens
-> free tokens / line
```

Pas encore branché dans ce test :

```text
remove_quotes_from_tokens
parse_tokens
launch_exec
```

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

Continuer dans `tests/test_loop.c`, pas dans le vrai `main.c`.

Ordre conseillé :

```text
1. ajouter remove_quotes_from_tokens après expand_tokens
2. afficher les tokens nettoyés
3. ajouter parse_tokens
4. afficher cmd_and_args
5. seulement ensuite tester launch_exec sur une commande externe simple
```

Ne pas brancher définitivement dans le main loop sans accord avec Dounia.
