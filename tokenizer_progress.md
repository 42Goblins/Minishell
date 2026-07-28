# Avancement du tokenizer — 29 juillet 2026

## Ce qui a été fait

Une première version du tokenizer est fonctionnelle. Elle transforme une ligne
simple en liste chaînée de `t_token`.

Cas actuellement compris :

```sh
echo hello
echo hello | wc
echo hello|wc
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

## Organisation des fichiers

### `srcs/lexer/lexer_nodes.c`

- `create_token_node` alloue et initialise un token.
- `add_token_back` ajoute un token à la fin de la liste.
- `free_tokens` libère la valeur et le nœud de chaque token.

Le token devient propriétaire de la chaîne placée dans `value`. C'est donc
`free_tokens` qui doit finalement libérer cette chaîne.

### `srcs/lexer/lexer.c`

- `add_word_token` crée un token `T_WORD`.
- `add_pipe_token` crée un token `T_PIPE`.
- `add_next_token` choisit quel token créer et indique combien de caractères
  ont été consommés.
- `tokenizer_error` nettoie une liste partielle en cas d'erreur.
- `tokenizer` parcourt la ligne avec `i`, qui reste uniquement l'index courant.

### `srcs/lexer/lexer_handlers.c`

- `is_blank` reconnaît un espace ou une tabulation.
- `word_length` calcule la longueur du prochain mot sans modifier l'index.

### `srcs/lexer/lexer_debug.c`

- `print_tokens` affiche temporairement le type numérique et la valeur de
  chaque token.

### `tests/test_lexer.c`

Ce fichier teste le tokenizer sans modifier le `main.c` utilisé par Dounia. Il
tokenize `echo hello|wc`, affiche la liste, puis la libère.

## Lancer le test

Construire d'abord la Libft si nécessaire :

```sh
make -C libft
```

Compiler le test :

```sh
cc -Wall -Wextra -Werror -Iinclude -Ilibft tests/test_lexer.c srcs/lexer/lexer.c srcs/lexer/lexer_handlers.c srcs/lexer/lexer_nodes.c srcs/lexer/lexer_debug.c libft/libft.a -o /tmp/test_lexer
```

Puis l'exécuter :

```sh
/tmp/test_lexer
```

Résultat attendu :

```text
type: 0, value: echo
type: 0, value: hello
type: 1, value: |
type: 0, value: wc
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

## Prochaine étape

Ajouter progressivement les redirections :

```text
<   >   <<   >>
```

Les quotes et l'expansion viendront seulement après les opérateurs simples.
