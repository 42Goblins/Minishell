# Reprise du travail sur Minishell

Ce document donne le contexte nécessaire pour reprendre le travail dans une
nouvelle conversation, sur une autre machine, sans devoir tout réexpliquer.

## 1. Contexte du projet

Projet Minishell de 42 réalisé en binôme :

- Chloé (`cmauley`) travaille sur la tokenisation, les quotes, l'expansion et
  le parsing.
- Dounia travaille principalement sur l'environnement, les builtins et l'exec.
- Les bonus ne sont pas faits pour le moment.

Chloé travaille sur la branche `chloe`.

Règle importante : ne pas supprimer ou réécrire le travail de Dounia sans
nécessité. Les tests du lexer restent dans `tests/` afin de ne pas modifier son
`main.c`.

## 2. Références à utiliser

Références officielles :

- sujet Minishell obligatoire : `/home/gpalemo/en.subject (1).pdf` ;
- Norme officielle : `/home/gpalemo/en.norm.pdf`.

Projet de référence des amis :

<https://github.com/Jdavid765/minishell/tree/niko>

La branche `niko` sert de référence principale pour la logique de tokenisation,
d'expansion, de parsing et pour le contrat envoyé à l'exec de Dounia. Il ne faut
pas copier aveuglément ce projet : proposer une adaptation plus claire si elle
est mieux adaptée au code actuel, en expliquant pourquoi.

Points déjà observés chez Nico :

- le lexer garde d'abord les quotes dans les tokens ;
- `had_quotes` sert plus tard pour l'expansion et les heredocs ;
- les quotes sont retirées dans une étape quote/expansion, pas directement au
  moment de créer le token ;
- Nico passe souvent `int *i`, mais Chloé préfère garder `i` comme index simple
  et retourner la longueur consommée.

## 3. Manière de travailler demandée

- Avancer linéairement, par petites étapes logiques.
- Expliquer le raisonnement avant de donner du code.
- Agir comme un professeur : Chloé doit comprendre et pouvoir expliquer le code
  pendant l'évaluation.
- Ne pas donner directement tout le code sauf si elle le demande.
- Garder `i` comme un simple index entier dans le tokenizer.
- Les commentaires `@brief` sont courts, utiles, placés au-dessus des fonctions
  et écrits en anglais conformément à la Norme.
- Chloé aime que les fichiers soient rangés pour lecture top-down :
  prototypes `static` en haut, fonction principale tôt dans le fichier, puis
  helpers dans l'ordre logique d'appel.
- Ne pas corriger le code de Dounia pour des problèmes sans rapport avec la
  partie lexer/parsing.

Ordre de priorité pour les décisions :

1. Sujet officiel.
2. Norme officielle.
3. Contrat avec l'exec de Dounia.
4. Logique du projet de Nico.
5. Adaptations pédagogiques clairement annoncées.

## 4. État Git actuel

Dernier état observé le 8 août 2026 :

```text
chloe est alignée avec origin/chloe au commit :
454a023 feat#21: handle basic lexer quotes
```

Mais le working tree contient du travail non commité :

```text
 M include/minishell.h
 D notes_chloe.md
 M tests/test_lexer.c
?? .vscode/
?? srcs/lexer/lexer_quotes.c
```

Notes :

- `.vscode/` est non suivi et ne doit pas être ajouté sans demande explicite.
- `notes_chloe.md` est supprimé localement ; ne pas le restaurer ou le supprimer
  définitivement sans vérifier l'intention de Chloé.
- `srcs/lexer/lexer_quotes.c` est nouveau et contient du travail en cours.

Remote après `git fetch --prune origin` :

```text
origin/dev    ea2703f feat: #3 export: added .h changes
origin/chloe  454a023 feat#21: handle basic lexer quotes
```

`origin/dev` contient du travail récent de Dounia sur `export`, `unset`, `exit`,
`exec`, `env` et une restructuration importante de `libft`. Un futur merge avec
`dev` aura presque sûrement un conflit dans `include/minishell.h`.

## 5. État actuel du tokenizer

Le tokenizer transforme une ligne en liste chaînée de `t_token`.

Types reconnus :

```text
T_WORD
T_PIPE
T_REDIR_IN
T_REDIR_OUT
T_APPEND
T_HEREDOC
```

Cas actuellement compris :

```sh
echo hello
echo hello | wc
echo hello|wc
cat<input
echo hello>output
cat<<EOF
echo hello>>output
echo 'hello world'
echo "hello world"
echo hello" world"
echo "hello"world
echo abc"def"ghi
```

Les quotes simples et doubles :

- empêchent les espaces de couper un `T_WORD` ;
- peuvent être collées à du texte ;
- restent pour l'instant dans `token->value` ;
- font passer `token->had_quotes` à `true` si le mot contient `'` ou `"`.

Les quotes non fermées sont détectées :

```sh
echo "hello
echo 'hello
```

Dans ce cas, `word_length` retourne `-1`, `tokenize_current_char` propage `-1`,
et `tokenizer` nettoie les tokens partiels via `tokenizer_error`.

## 6. Structures et contrat mémoire

Le token actuel :

```c
typedef struct s_token
{
	t_token_type	type;
	char			*value;
	bool			had_quotes;
	struct s_token	*next;
}					t_token;
```

Contrat mémoire :

```text
ft_substr / ft_strdup / remove_quotes allouent une string
-> le token devient propriétaire de value
-> free_tokens libère value puis le token
```

`create_token_node` initialise :

```text
type
value
had_quotes = false
next = NULL
```

Quand `add_word_token` crée un `T_WORD`, il met `had_quotes = true` si `value`
contient `'` ou `"`.

## 7. Organisation actuelle des fichiers lexer

### `srcs/lexer/lexer.c`

Contient la boucle principale et le dispatch général.

Ordre actuel voulu par Chloé :

```text
tokenizer
tokenize_current_char
tokenizer_error
add_operator_token
add_word_token
```

Rôles :

- `tokenizer` parcourt l'input avec `i`.
- `tokenize_current_char` choisit quoi tokeniser à l'index courant.
- `tokenizer_error` nettoie une liste partielle en cas d'erreur.
- `add_operator_token` crée un token opérateur (`|`, `<`, `>`, `<<`, `>>`).
- `add_word_token` extrait un `T_WORD` et pose `had_quotes` si nécessaire.

### `srcs/lexer/lexer_redir.c`

Gère les redirections, proche de l'esprit Nico mais avec retour de longueur :

- `add_redir_in_or_heredoc` choisit entre `<` et `<<`.
- `add_redir_out_or_append` choisit entre `>` et `>>`.

Ces fonctions retournent :

```text
1 ou 2 = nombre de caractères consommés
-1 = erreur d'allocation
```

### `srcs/lexer/lexer_handlers.c`

Contient les helpers de lecture :

- `is_blank` reconnaît espace et tabulation.
- `word_length` calcule la longueur du prochain mot.
- `quoted_word_length` calcule une portion entre quotes et retourne `-1` si la
  quote fermante manque.

`word_length` s'arrête sur :

```text
blank, |, <, >
```

mais saute correctement les espaces situés dans quotes.

### `srcs/lexer/lexer_nodes.c`

Contient :

- `create_token_node`
- `add_token_back`
- `free_tokens`

### `srcs/lexer/lexer_quotes.c`

Nouveau fichier de travail pour le traitement des quotes après tokenisation.

Fonctions présentes :

- `remove_quotes` : fonction publique qui retourne une nouvelle string sans les
  quotes actives.
- `stripped_length` : helper `static` qui calcule la taille après retrait des
  quotes actives.

Exemples validés en test isolé :

```text
"hello world" -> hello world
'hello world' -> hello world
abc"def"ghi -> abcdefghi
"\'hello\'" -> 'hello'
'\"hello\"' -> "hello"
```

Attention : au dernier état observé, `lexer_quotes.c` contient aussi une
fonction commencée mais incomplète :

```c
int	remove_quotes_from_tokens(t_token *tokens)
```

Elle ne compile pas encore et doit être terminée ou retirée avant de compiler
avec `srcs/lexer/lexer_quotes.c`.

### `tests/test_lexer.c`

Test isolé du tokenizer. Il ne modifie pas `main.c`.

Contient :

- `print_tokens` local au test ;
- `test_input` pour les entrées valides ;
- `test_invalid_input` pour les entrées qui doivent échouer ;
- `test_remove_quotes` pour tester `remove_quotes` isolément.

`lexer_debug.c` a été supprimé, car `print_tokens` ne sert qu'aux tests.

## 8. Compiler et lancer les tests

Construire la Libft si nécessaire :

```sh
make -C libft
```

Compiler le test actuel en incluant `lexer_quotes.c` seulement si la fonction
WIP `remove_quotes_from_tokens` est terminée ou retirée :

```sh
cc -Wall -Wextra -Werror -Iinclude -Ilibft tests/test_lexer.c srcs/lexer/lexer.c srcs/lexer/lexer_redir.c srcs/lexer/lexer_handlers.c srcs/lexer/lexer_nodes.c srcs/lexer/lexer_quotes.c libft/libft.a -o /tmp/test_lexer
```

Lancer :

```sh
/tmp/test_lexer
```

Si `remove_quotes_from_tokens` est encore incomplète, compiler sans
`lexer_quotes.c` ne testera pas `remove_quotes`; il vaut mieux terminer ou
commenter cette fonction incomplète plutôt que contourner durablement le
problème.

## 9. État du travail de Dounia utile pour l'expansion

Après fetch, `origin/dev` contient maintenant côté env/builtins :

```text
setup_env
new_env_node
cpy_key
cpy_value
get_env_value
set_env_value
update_env
exec_env
exec_export
parse_export
update_env_vars
add_new_var
print_export
```

Pour l'expansion `$VAR`, Chloé doit lire l'env sans la modifier :

```c
get_env_value(shell->env, "HOME")
```

Contrat souhaité :

```text
Dounia maintient shell->env via env/export/unset/cd.
Chloé lit shell->env pour remplacer $VAR pendant l'expansion.
```

Ne pas dupliquer une fonction de recherche env côté parsing.

Point à décider plus tard avec Dounia :

```text
où stocker le dernier exit status pour gérer $?
```

Pour l'instant, `t_shell` ne contient pas encore de champ clair du style
`last_status`.

## 10. Prochaine étape exacte

Reprendre à `srcs/lexer/lexer_quotes.c`.

Objectif immédiat :

```text
terminer remove_quotes_from_tokens
```

Logique voulue :

```text
current = tokens
while current
    if current->type == T_WORD && current->had_quotes
        new_value = remove_quotes(current->value)
        if !new_value
            return 1
        free(current->value)
        current->value = new_value
    current = current->next
return 0
```

Quand cette fonction compile :

1. Ajouter son prototype dans `include/minishell.h`.
2. Ajouter un test dédié dans `tests/test_lexer.c`.
3. Ne pas encore l'appeler automatiquement dans `tokenizer`.
4. Tester d'abord sur une liste de tokens déjà créée :

```text
tokenizer("echo \"hello world\"", &shell)
print avant remove_quotes_from_tokens
remove_quotes_from_tokens(shell.token)
print après
free_tokens(shell.token)
```

Résultat attendu :

```text
avant : value: "hello world", had_quotes: 1
après : value: hello world, had_quotes: 1
```

Important : ne pas attaquer l'expansion `$VAR` tant que le retrait des quotes
sur tokens n'est pas testé proprement.

## 11. Reprendre sur un autre ordinateur

Après avoir ouvert le dépôt :

```sh
git fetch --prune origin
git switch chloe
git pull --ff-only origin chloe
git status
```

Si le travail non commité de cette machine n'a pas encore été push, il faut le
récupérer ou le refaire avant de continuer. Vérifier en priorité :

```text
srcs/lexer/lexer_quotes.c
include/minishell.h
tests/test_lexer.c
```

Message à donner à une nouvelle conversation :

```text
Lis entièrement HANDOFF.md et tokenizer_progress.md, puis inspecte l'état actuel
des fichiers du lexer. Reprends à la section « Prochaine étape exacte ». Ne
modifie pas le travail de Dounia sans lien direct avec le lexer/parsing. Guide-moi
pédagogiquement, par petites étapes, sans me donner tout le code sauf demande
explicite.
```
