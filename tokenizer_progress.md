# Avancement du tokenizer — 5 août 2026

## Ce qui a été fait

Une première version du tokenizer est fonctionnelle. Elle transforme une ligne
simple en liste chaînée de `t_token`.

Cas actuellement compris :

```sh
echo hello
echo hello | wc
echo hello|wc
cat<input
echo hello>output
cat<<EOF
echo hello>>output
```

Pour `echo hello|wc`, la liste obtenue est :

```text
T_WORD("echo")
-> T_WORD("hello")
-> T_PIPE("|")
-> T_WORD("wc")
-> NULL
```

Les espaces et les tabulations séparent les mots. Le pipe est reconnu même
lorsqu'il n'y a pas d'espace autour.

Les redirections sont maintenant reconnues comme des tokens séparés, même
lorsqu'elles sont collées aux mots :

```text
T_REDIR_IN("<")
T_REDIR_OUT(">")
T_HEREDOC("<<")
T_APPEND(">>")
```

## Organisation des fichiers

### `srcs/lexer/lexer_nodes.c`

- `create_token_node` alloue et initialise un token.
- `add_token_back` ajoute un token à la fin de la liste.
- `free_tokens` libère la valeur et le nœud de chaque token.

Le token devient propriétaire de la chaîne placée dans `value`. C'est donc
`free_tokens` qui doit finalement libérer cette chaîne.

### `srcs/lexer/lexer.c`

- `add_word_token` crée un token `T_WORD`.
- `add_operator_token` crée un token opérateur à partir d'un type et d'une
  chaîne.
- `tokenize_current_char` choisit quel token créer à l'index courant et indique
  combien de caractères ont été consommés.
- `tokenizer_error` nettoie une liste partielle en cas d'erreur.
- `tokenizer` parcourt la ligne avec `i`, qui reste uniquement l'index courant.

Le fichier est rangé pour lire d'abord la fonction principale :

```text
tokenizer
tokenize_current_char
tokenizer_error
add_operator_token
add_word_token
```

### `srcs/lexer/lexer_redir.c`

- `add_redir_in_or_heredoc` choisit entre `<` et `<<`.
- `add_redir_out_or_append` choisit entre `>` et `>>`.

Ces fonctions retournent `1` ou `2` selon le nombre de caractères consommés, et
`-1` en cas d'erreur d'allocation.

### `srcs/lexer/lexer_handlers.c`

- `is_blank` reconnaît un espace ou une tabulation.
- `word_length` calcule la longueur du prochain mot sans modifier l'index.
  Un mot s'arrête maintenant sur un blank, `|`, `<` ou `>`.

### `tests/test_lexer.c`

Ce fichier teste le tokenizer sans modifier le `main.c` utilisé par Dounia. Il
tokenize plusieurs lignes, affiche la liste, puis la libère. Le helper
temporaire `print_tokens` vit directement dans ce fichier de test.

## Lancer le test

Construire d'abord la Libft si nécessaire :

```sh
make -C libft
```

Compiler le test :

```sh
cc -Wall -Wextra -Werror -Iinclude -Ilibft tests/test_lexer.c srcs/lexer/lexer.c srcs/lexer/lexer_redir.c srcs/lexer/lexer_handlers.c srcs/lexer/lexer_nodes.c libft/libft.a -o /tmp/test_lexer
```

Puis l'exécuter :

```sh
/tmp/test_lexer
```

Exemple de résultat attendu pour `echo hello>>output` :

```text
type: 0, value: echo
type: 0, value: hello
type: 4, value: >>
type: 0, value: output
```

## Rapport avec le code de Nico

La logique générale reste proche de la sienne : lecture de la ligne, création
d'une liste de tokens, séparation des mots et des opérateurs, puis stockage de
la liste dans la structure principale du shell.

Quelques adaptations ont été faites pour avancer progressivement :

- `i` reste un index simple et n'est pas transmis comme `int *` ;
- la liste est encore simplement chaînée ;
- les champs liés aux quotes et à l'expansion ne sont pas encore ajoutés ;
- les allocations partielles sont nettoyées si le tokenizer échoue.
- contrairement à Nico, les fonctions de lecture ne modifient pas `i`
  directement ; elles retournent la longueur consommée.

## Prochaine étape

Vérifier la Norminette sur les fichiers du lexer, puis passer aux quotes simples
et doubles. L'expansion viendra après les quotes.
