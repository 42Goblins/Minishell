# Avancement de l'expansion — 16 août 2026

## Objectif

Pipeline retenue :

```text
tokens avec quotes conservées
-> expansion de $VAR / $? / $digit en respectant les quotes
-> retrait des quotes avec remove_quotes_from_tokens
-> validation syntaxique
-> parser vers t_cmd
-> exec
```

Les quotes restent présentes pendant l'expansion. Elles sont retirées après,
sinon on perdrait la différence entre single quotes et double quotes.

Règles principales :

```sh
$USER       # expansion
"$USER"     # expansion
'$USER'     # pas d'expansion
$?          # dernier status
$2USER      # $2 vaut vide, USER reste littéral
```

## Fichiers concernés

```text
srcs/expansion/expansion.c
srcs/expansion/expansion_vars.c
srcs/expansion/expansion_utils.c
srcs/utils/get_status.c
tests/test_expansion.c
```

Le code de retrait des quotes est dans :

```text
srcs/lexer/lexer_quotes.c
```

## Architecture actuelle

```text
expansion.c
```

Contient la logique principale :

- `expand_word`
- `update_quote_state`
- `replace_current_var`
- `join_three_parts`

```text
expansion_vars.c
```

Contient la logique liée aux variables :

- `is_dollar_expand`
- `get_var_value`
- `var_name_len`
- `get_var_name`
- `is_var_char`

```text
expansion_utils.c
```

Contient les helpers techniques :

- `free_three_strings`
- `append_expansion_part`

```text
get_status.c
```

Contient `get_status`, utilisé par `$?`.

## Décision importante

La logique suit l'esprit de Nico sans être un copier-coller :

```text
result = ft_strdup(word)
scanner result
quand un $ expandable est trouvé :
    remplacer directement dans result
reprendre le scan après la valeur insérée
return result
```

Cette approche évite l'ancienne logique `built/start`, qui devenait difficile à
lire.

## Fonctions terminées

### `expand_word`

Scanne un mot complet avec ses quotes encore présentes.

La boucle fait trois choses :

- met à jour l'état single/double quotes ;
- remplace un `$` expandable si le contexte l'autorise ;
- avance au caractère suivant sinon.

### `replace_current_var`

Remplace une expansion trouvée à l'index du `$`.

Découpage :

```text
before = tout avant le $
value  = valeur de $VAR, $? ou $digit
after  = tout après la partie consommée
```

Exemple :

```text
result = "hello $USER!"

before = "hello "
value  = "chloe"
after  = "!"

new_result = "hello chloe!"
```

`new_i` indique où `expand_word` doit reprendre après remplacement.

### `is_dollar_expand`

Décide si le `$` courant doit déclencher une expansion.

Elle accepte :

- `$VAR`
- `$?`
- `$digit`

Elle refuse :

- `$` dans des single quotes ;
- `$` seul ;
- `$` suivi d'un caractère non supporté comme `-` ou `.`.

### `get_var_value`

Retourne une nouvelle string allouée.

Contrat mémoire :

```text
get_env_value -> adresse empruntée à shell->env, ne jamais free
get_var_value -> nouvelle chaîne allouée, le caller doit free
```

Cas gérés :

```text
USER    -> valeur de USER dans env
?       -> ft_itoa(*get_status())
2USER   -> "" parce que $2 vaut vide dans notre minishell
MISSING -> ""
invalide -> NULL
```

### `var_name_len`

Mesure un nom de variable classique.

Règles :

- premier caractère : lettre ou `_` ;
- caractères suivants : alphanumériques ou `_`.

Exemples :

```text
USER/test -> 4
USER2!    -> 5
_NAME=    -> 5
2USER     -> 0
```

### `get_status`

Retourne l'adresse d'un `static int status`.

Contrat décidé avec Dounia :

```text
exec / builtins / erreurs / signaux -> écrivent *get_status()
expansion de $?                    -> lit *get_status()
```

Valeur initiale : `0`.

## Cas testés actuellement

Les tests dans `tests/test_expansion.c` couvrent :

```text
$USER              -> chloe
'$USER'            -> '$USER'
"$USER"            -> "chloe"
abc$USER           -> abcchloe
"it's $USER"       -> "it's chloe"
'"$USER"'          -> '"$USER"'
$?                 -> 127
status:$?          -> status:127
"$?"               -> "127"
'$?'               -> '$?'
$?$USER            -> 127chloe
$MISSING           -> ""
a$MISSINGb         -> a
$USER$HOME         -> chloe/home/chloe
$?abc              -> 127abc
$USER?             -> chloe?
"$USER$?"          -> "chloe127"
'$USER'$HOME       -> '$USER'/home/chloe
$2USER             -> USER
$12USER            -> 2USER
$9abc              -> abc
$1                 -> ""
```

## Commande de test

```sh
make -C libft
cc -Wall -Wextra -Werror \
-Iinclude -Ilibft/inc \
tests/test_expansion.c \
srcs/expansion/expansion.c \
srcs/expansion/expansion_vars.c \
srcs/expansion/expansion_utils.c \
srcs/builtins/cd.c \
srcs/utils/get_status.c \
libft/libft.a \
-o /tmp/test_expansion
/tmp/test_expansion
```

Dernier état connu : la compilation passe et tous les tests expansion passent.

## Ce qui reste à faire

### 1. Brancher l'expansion sur les tokens

Créer une fonction qui parcourt la liste de tokens et applique `expand_word` aux
tokens concernés.

Idée :

```text
pour chaque token
    si token WORD doit être expand
        token->value = expand_word(token->value, shell->env)
```

Ne pas encore mélanger heredoc dans cette étape.

### 2. Valider l'ordre avec le retrait des quotes

L'ordre doit rester :

```text
expansion
-> remove_quotes_from_tokens
```

À tester avec :

```sh
echo "$USER"
echo '$USER'
echo '$USER'$HOME
echo "$USER$?"
```

### 3. Ajouter des tests pipeline

Pour l'instant, les tests appellent surtout `expand_word` directement.

Prochaine couche de tests :

```text
input brut
-> tokenizer
-> expansion sur tokens
-> remove_quotes_from_tokens
-> print tokens
```

### 4. Redirections

À valider au moment de l'intégration :

```sh
echo hi > $FILE
cat < "$INPUT"
```

Les noms de fichiers doivent être expandés puis débarrassés de leurs quotes.

### 5. Heredoc

À traiter séparément.

Règle :

```sh
cat << EOF      # contenu heredoc expandé
cat << "EOF"    # contenu heredoc non expandé
```

Le delimiter doit perdre ses quotes, mais `had_quotes` doit rester disponible
pour décider si le contenu du heredoc doit être expandé.

### 6. Brancher réellement `get_status`

`$?` lit déjà `get_status`, mais il faudra que Dounia branche les écritures :

```text
commande réussie -> *get_status() = 0
commande introuvable -> *get_status() = 127
Ctrl-C -> *get_status() = 130
```

### 7. Refacto / Norm

`expand_word` est encore un peu longue.

Décision actuelle : ne pas refacto lourdement tant que l'intégration tokens /
quotes / redirections n'est pas stabilisée.

À faire plus tard :

- réduire `expand_word` si nécessaire ;
- relancer `norminette` sur les fichiers expansion ;
- garder le découpage compréhensible avant de chercher une Norm parfaite.

## À ne pas faire maintenant

- Ne pas gérer `$$` comme PID.
- Ne pas gérer `$-` comme bash complet.
- Ne pas gérer `${VAR}`.
- Ne pas retirer les quotes avant expansion.
- Ne pas refacto lourdement `expand_word` avant l'intégration.
