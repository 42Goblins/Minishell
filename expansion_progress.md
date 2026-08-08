# Avancement de l'expansion — 9 août 2026

## Objectif et ordre des étapes

```text
tokens avec quotes
-> expansion de $VAR selon les quotes
-> retrait des quotes
-> parser
```

Règles retenues :

```sh
$USER       # expansion
"$USER"     # expansion
'$USER'     # pas d'expansion
```

Les quotes restent présentes pendant l'expansion. Elles seront retirées ensuite
par `remove_quotes_from_tokens`.

## Fichiers

```text
srcs/expansion/expansion_handlers.c
srcs/expansion/expansion.c
tests/test_expansion.c
```

## Fonctions terminées

### `var_name_len`

Mesure le nom situé juste après `$`. Le premier caractère doit être une lettre
ou `_`. Les suivants peuvent être alphanumériques ou `_`.

```text
USER/test -> 4
USER2!    -> 5
_NAME=    -> 5
2USER     -> 0
```

### `get_var_name` — static

Extrait le nom avec `ft_substr`. La fonction est interne à
`expansion_handlers.c` et la chaîne créée doit être libérée.

### `get_var_value`

Extrait le nom, consulte `get_env_value`, libère le nom puis retourne une copie
allouée de la valeur.

```text
get_env_value -> adresse empruntée à shell->env, ne pas la free
get_var_value -> nouvelle chaîne allouée, le caller doit la free
variable absente -> nouvelle chaîne vide
nom invalide -> NULL
```

### `is_dollar_expand`

Indique si le `$` à l'index courant commence une variable expansible. Elle
refuse un `$` dans des single quotes ou sans nom valide derrière.

`$?` n'est pas encore traité.

### `append_expansion_part`

Assemble la chaîne déjà construite (`built`) avec un nouveau morceau (`part`).
Elle libère toujours les deux anciennes chaînes et retourne le résultat de
`ft_strjoin`.

Il faut lui passer des chaînes allouées, jamais des littéraux :

```c
append_expansion_part(ft_strdup("hello "), ft_strdup("chloe"));
```

## État actuel de `expand_word`

La fonction existe, mais elle ne remplace pas encore les variables.

Elle sait déjà :

- parcourir le mot avec `i` ;
- suivre `in_single` et `in_double` ;
- ignorer une single quote dans des doubles quotes ;
- ignorer une double quote dans des single quotes ;
- reconnaître un `$` expansible.

Elle retourne encore `ft_strdup(word)`. Des commentaires `// TODO` temporaires
en français indiquent les étapes restantes. Ils feront échouer la Norm jusqu'à
leur suppression ou conversion.

## Prochaine micro-étape exacte

Ne faire que l'initialisation de la construction :

1. ajouter `int start` et `char *built` dans `expand_word` ;
2. initialiser `start` à `0` ;
3. créer `built` avec `ft_strdup("")` ;
4. vérifier l'échec d'allocation ;
5. ne pas encore remplir le bloc du `$`.

```text
i     -> caractère actuellement lu
start -> début du texte pas encore copié
built -> chaîne construite jusque-là
```

## Tests actuels

`tests/test_expansion.c` contient des sections lisibles pour :

- les longueurs de noms ;
- la récupération des valeurs dans un environnement contrôlé ;
- la décision d'expand ou non un `$` ;
- le parcours avec single et double quotes ;
- l'assemblage de morceaux alloués.

Dernière vérification Valgrind :

```text
25 allocations, 25 frees
0 byte restant
0 erreur
```

Compiler :

```sh
cc -Wall -Wextra -Werror \
-Iinclude -Ilibft/inc \
tests/test_expansion.c \
srcs/expansion/expansion.c \
srcs/expansion/expansion_handlers.c \
srcs/builtins/cd.c \
libft/libft.a \
-o /tmp/test_expansion

/tmp/test_expansion
```

Valgrind :

```sh
valgrind --leak-check=full \
--show-leak-kinds=all \
--errors-for-leak-kinds=definite \
/tmp/test_expansion
```

## À valider avec Dounia

- corriger la comparaison exacte des clés dans `get_env_value` ;
- ajouter et maintenir un futur `last_status` pour `$?` ;
- confirmer le contrat heredoc lié aux quotes du délimiteur.

Ces points n'empêchent pas de terminer l'expansion classique de `$VAR`.
