# HANDOFF — Minishell

Dernière mise à jour : 1 septembre 2026, branche `chloe`.

## Contexte

Projet 42 minishell, mandatory uniquement.

Répartition actuelle :

```text
Moi    : lexer, tokenisation, expansion, retrait des quotes, syntaxe, parser.
Dounia : env, builtins, exec, fd, heredoc, signaux, status.
```

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
chloe reste ma branche atelier avec les .md et tests temporaires.
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
Dounia dans `srcs/main.c`. Les tests locaux peuvent définir une petite version
locale de `get_status()` dans leur propre fichier pour éviter de compiler deux
`main`.

Point signalé à Dounia : `get_env_value` / `set_env_value` doivent comparer les
clés avec `ft_strcmp(...) == 0`. Sinon `$USER` peut récupérer la mauvaise valeur
d'environnement.

## Parser

État : parser commande simple + validation syntaxique de base en place.

Fichiers :

```text
srcs/parser/parser.c
srcs/parser/parser_redir.c
srcs/parser/syntax.c
srcs/parser/parser_utils.c
```

Voir aussi `parser_progress.md` pour le détail de ce qui est fait et de ce qui
reste.

Fonctions actuelles :

```text
parse_tokens
create_cmd_node
create_cmd_and_args
count_cmd_args
validate_syntax
is_redirection_token
```

Ce qui marche :

```text
tokens WORD préparés
-> t_cmd
-> cmd_and_args dupliqué
redirections et leur filename/delimiter ignorés dans cmd_and_args
syntax errors évidentes refusées avant parse_tokens
```

Exemple :

```text
echo hello -> ["echo", "hello", NULL]
echo hi > out -> ["echo", "hi", NULL]
cat << EOF -> ["cat", NULL]
```

Limites actuelles :

```text
pas encore de parsing complet des pipes
pas encore de redirections stockées dans t_cmd
pas encore de détection builtin
pas encore de messages bash-like/status 2 centralisés pour syntax errors
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
compiler un test sans le vrai `main`, ajouter une petite version locale de
`get_status()` dans le fichier de test, comme dans `tests/test_loop.c`.

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

Ce test couvre actuellement :

```text
readline
-> add_history
-> tokenizer
-> expand_tokens
-> remove_quotes_from_tokens
-> validate_syntax
-> parse_tokens
-> print tokens + cmd_and_args
-> free cmd / tokens / line
```

Pas encore branché dans ce test :

```text
launch_exec
```

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
test_parser : redirections PASS, $USER/$MISSING encore FAIL à cause de get_env_value
```

## Contrat avec Dounia

Voir `contract_avec_dounia.md`.

Points à valider ensemble :

- structure exacte attendue par l'exec ;
- format de `t_cmd` / `cmd_and_args` ;
- gestion des redirections ;
- qui ouvre les fichiers ;
- qui lit le heredoc ;
- où écrire `*get_status()` ;
- qui centralise les messages d'erreur.

## Erreurs

Répartition décidée provisoirement :

```text
Ma partie détecte les erreurs avant exec :
- quote non fermée
- pipe mal placé
- redirection sans filename
- token inattendu

Dounia détecte les erreurs d'exec :
- command not found
- permission denied
- open / dup / fork / execve / waitpid
- builtins
- signaux
```

Codes à garder :

```text
syntax error -> 2
command not found -> 127
permission denied -> 126
redirection open fail -> 1
Ctrl-C -> 130
Ctrl-\ -> 131
```

Décision temporaire pour avancer :

```text
validate_syntax retourne 0 si OK, 1 si erreur.
Les messages exacts et *get_status() seront centralisés quand la vraie boucle
principale sera décidée avec Dounia.
```

## Prochaine étape

Continuer dans `tests/test_loop.c`, pas dans le vrai `main.c`.

Ordre conseillé maintenant :

```text
1. parser les pipes vers plusieurs t_cmd chaînés
2. décider avec Dounia comment stocker/ouvrir les redirections dans t_cmd
3. gérer heredoc avec Dounia
4. centraliser messages d'erreur + *get_status()
```

Ne pas brancher définitivement dans le main loop sans accord avec Dounia.
