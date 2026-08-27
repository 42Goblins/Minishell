# Tokenizer terminé — 19 août 2026

## Statut

La tokenisation obligatoire de minishell est terminée et testée. Elle transforme
une ligne en liste chaînée de `t_token`, sans effectuer l'expansion ni le
parsing.

```text
ligne
-> tokenizer
-> expansion
-> retrait des quotes
-> vérification de syntaxe et parsing
-> exec
```

Le tokenizer conserve volontairement les quotes dans `token->value`.

État d'intégration :

```text
La PR clean contenant le lexer/tokenizer a été mergée dans dev.
La branche chloe garde les docs et tests temporaires.
tests/test_loop.c utilise maintenant tokenizer dans une mini boucle readline.
```

## Tokens reconnus

```text
T_WORD       mot
T_PIPE       |
T_REDIR_IN   <
T_REDIR_OUT  >
T_HEREDOC    <<
T_APPEND     >>
```

Les opérateurs sont séparés même lorsqu'ils sont collés aux mots :

```sh
echo hello|wc
cat<input
echo hello>output
cat<<EOF
echo hello>>output
```

Les espaces et tabulations séparent les mots. Les opérateurs entre quotes
restent du texte dans un `T_WORD`.

## Quotes

Les single et double quotes empêchent la séparation du mot :

```sh
echo 'hello world'
echo "hello world"
echo abc"def"ghi
echo "" ''
```

Une quote non fermée fait échouer le tokenizer et la liste partiellement créée
est libérée.

Chaque token contient `bool had_quotes`. Ce champ devient vrai si le mot
contenait une quote et reste vrai après son retrait, notamment pour conserver
l'information nécessaire au heredoc.

## Organisation

### `srcs/lexer/lexer.c`

- `tokenizer` parcourt la ligne ;
- `tokenize_current_char` choisit le token à créer ;
- `add_word_token` extrait un mot et pose `had_quotes` ;
- `add_operator_token` crée un opérateur ;
- `tokenizer_error` libère une liste partielle.

`i` reste uniquement l'index courant. Les handlers retournent le nombre de
caractères consommés.

### `srcs/lexer/lexer_utils.c`

- `is_blank` reconnaît espace et tabulation ;
- `word_len` mesure le prochain mot ;
- `quoted_word_len`, interne au fichier, cherche la quote fermante.

### `srcs/lexer/lexer_redir.c`

- `add_redir_in_or_heredoc` distingue `<` et `<<` ;
- `add_redir_out_or_append` distingue `>` et `>>`.

### `srcs/lexer/lexer_nodes.c`

- `create_token_node` alloue et initialise un token ;
- `add_token_back` ajoute un token en fin de liste ;
- `free_tokens` libère les valeurs et les nœuds.

Le token devient propriétaire de la chaîne placée dans `value`.

La liste de tokens est maintenant doublement chaînée :

```text
prev <- current -> next
```

`prev` est utilisé par `expand_tokens` pour ne pas expand le delimiter heredoc,
c'est-à-dire le `T_WORD` placé juste après `T_HEREDOC`.

### `srcs/lexer/lexer_quotes.c`

- `remove_quotes` crée une copie sans les quotes actives ;
- `remove_quotes_from_tokens` remplace les valeurs des `T_WORD` concernés.

Le retrait des quotes est terminé et testé, mais il n'est pas appelé dans le
tokenizer : il doit avoir lieu après l'expansion.

## Tests

`tests/test_lexer.c` couvre :

- mots, espaces et tabulations ;
- pipes et toutes les redirections ;
- opérateurs collés aux mots ;
- quotes simples, doubles, vides et mélangées ;
- opérateurs protégés par des quotes ;
- entrée vide et quotes non fermées ;
- retrait des quotes sur une chaîne et sur une liste de tokens.

Les suites comme `|||` ou `><` sont volontairement tokenisées : le parser devra
ensuite les refuser comme erreurs de syntaxe.

Dernière vérification complète :

```text
-Wall -Wextra -Werror : OK
Norminette sur srcs/lexer/*.c : OK
Valgrind : 121 allocations, 121 frees, 0 erreur
```

## Compiler les tests

```sh
make -C libft

cc -Wall -Wextra -Werror \
-Iinclude -Ilibft/inc \
tests/test_lexer.c \
srcs/lexer/lexer.c \
srcs/lexer/lexer_redir.c \
srcs/lexer/lexer_utils.c \
srcs/lexer/lexer_nodes.c \
srcs/lexer/lexer_quotes.c \
libft/libft.a \
-o /tmp/test_lexer

/tmp/test_lexer
```

## Conclusion

Le lexer est considéré terminé pour le mandatory. On n'y reviendra que si les
tests d'intégration révèlent un cas manquant.

Travail actif actuel :

```text
tests/test_loop.c
readline -> tokenizer -> expand_tokens -> prochainement remove_quotes_from_tokens
```
