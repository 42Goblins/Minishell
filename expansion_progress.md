# Avancement de l'expansion — 10 août 2026

## Objectif

Pipeline retenue :

```text
tokens avec quotes conservées
-> expansion de $VAR / $? en respectant les quotes
-> retrait des quotes avec remove_quotes_from_tokens
-> validation syntaxique
-> parser vers t_cmd
-> exec
```

Règles importantes :

```sh
$USER       # expansion
"$USER"     # expansion
'$USER'     # pas d'expansion
```

Les quotes restent donc présentes pendant l'expansion. Elles sont retirées
après, sinon on perdrait la différence entre single quotes et double quotes.

## Fichiers concernés

```text
srcs/expansion/expansion_handlers.c
srcs/expansion/expansion.c
tests/test_expansion.c
```

Le code de retrait des quotes est dans :

```text
srcs/lexer/lexer_quotes.c
```

## Fonctions terminées

### `var_name_len`

Mesure le nom situé juste après `$`.

Règles actuelles :

- premier caractère : lettre ou `_` ;
- caractères suivants : alphanumériques ou `_`.

Exemples :

```text
USER/test -> 4
USER2!    -> 5
_NAME=    -> 5
2USER     -> 0
```

### `get_var_value`

Extrait le nom de variable, appelle `get_env_value`, puis retourne une nouvelle
chaîne allouée.

Contrat mémoire :

```text
get_env_value -> adresse empruntée à shell->env, ne jamais free
get_var_value -> nouvelle chaîne allouée, le caller doit free
variable absente -> chaîne vide allouée
nom invalide -> NULL
```

### `is_dollar_expand`

Décide si le `$` à l'index courant commence une expansion.

Elle refuse :

- `$` dans des single quotes ;
- `$` sans nom valide derrière ;
- `$?` pour l'instant.

`$?` sera ajouté plus tard avec `get_status()`.

### `append_expansion_part`

Assemble deux chaînes allouées :

```text
built + part
```

Elle libère toujours les deux arguments. Ne jamais lui passer directement des
littéraux.

## Décision importante : approche façon Nico

On a commencé avec une approche `built/start`, mais elle devenait trop difficile
à lire.

Décision prise : repartir vers une logique plus proche de Nico.

Nouvelle logique prévue :

```text
result = ft_strdup(word)
scanner result
quand un $VAR expandable est trouvé :
    remplacer ce $VAR directement dans result
continuer à scanner la nouvelle string
return result
```

Exemple :

```text
result = "hello-$USER!"

before = "hello-"
value  = "chloe"
after  = "!"

result = before + value + after
result = "hello-chloe!"
```

## Où on en est

`expand_word` a été volontairement remise à une base simple :

```text
si word == NULL -> NULL
sinon -> ft_strdup(word)
```

Des commentaires `TODO` sont présents dans `srcs/expansion/expansion.c` pour
indiquer où reprendre.

Attention : la fonction `replace_current_var` est WIP. Comme elle est `static`
et pas encore appelée, `expansion.c` ne compilera pas proprement avec
`-Werror` tant qu'elle n'est pas branchée ou temporairement retirée.

## Fonction WIP : `replace_current_var`

But :

```text
remplacer le $VAR situé à l'index i dans result par sa valeur env
```

Signature actuelle :

```c
static char	*replace_current_var(char *result, int i, t_env *env, int *new_i)
```

Exemple :

```text
result = "hello-$USER!"
i      = index du $
env    = USER=chloe
```

Découpage :

```text
before = "hello-"
value  = "chloe"
after  = "!"
```

Puis :

```text
new_result = before + value + after
```

La fonction calcule déjà :

- `var_len` ;
- `before` ;
- `value` ;
- `after` ;
- `new_i` ;
- `new_result`.

Problème actuel :

- la fonction est trop longue ;
- le bloc de cleanup en cas d'erreur prend trop de place ;
- elle n'est pas encore appelée par `expand_word`.

## Où reprendre demain

Ne pas repartir sur toute la logique d'un coup.

Le point exact où reprendre est dans `replace_current_var`, sur le bloc :

```c
if (!before || !value || !after)
{
	free(before);
	free(value);
	free(after);
	free(result);
	return (NULL);
}
```

Ce bloc marche, mais il rend la fonction lourde. La prochaine étape sert juste à
le sortir dans un helper pour que `replace_current_var` redevienne lisible.

## Prochaine micro-étape exacte

Reprendre dans `srcs/expansion/expansion.c`.

Faire d'abord un petit helper de cleanup pour alléger `replace_current_var`.

Nom proposé :

```c
static char	*free_var_parts(char *before, char *value,
		char *after, char *result)
```

Rôle :

```text
free before
free value
free after
free result
return NULL
```

Ensuite remplacer ce bloc :

```c
if (!before || !value || !after)
{
	free(before);
	free(value);
	free(after);
	free(result);
	return (NULL);
}
```

par :

```text
if allocation failed
    return free_var_parts(...)
```

Après ça seulement :

1. vérifier que `replace_current_var` reste compréhensible ;
2. brancher `replace_current_var` dans `expand_word` ;
3. faire scanner `expand_word` avec `in_single` / `in_double` ;
4. tester `$USER`, `abc$USER`, `"$USER"` et `'$USER'`.

## Tests actuels

`tests/test_expansion.c` contient déjà des tests pour :

- `var_name_len` ;
- `get_var_value` ;
- `is_dollar_expand` ;
- `append_expansion_part` ;
- des scans quote-aware temporaires.

La section `QUOTE-AWARE WORD SCAN` n'est pas encore un vrai test du résultat de
`expand_word`, parce que `expand_word` retourne encore une copie du mot.

Commande de compilation prévue quand `replace_current_var` sera branchée :

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

## À ne pas faire tout de suite

- Ne pas commencer `$?` avant d'ajouter/utiliser `get_status()`.
- Ne pas retirer les quotes avant l'expansion.
- Ne pas intégrer à tout le pipeline tant que `expand_word` n'est pas testée
  seule.
- Ne pas corriger la Norm en supprimant les repères pédagogiques avant d'avoir
  fini la logique.
