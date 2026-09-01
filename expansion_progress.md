# Avancement de l'expansion — 1 septembre 2026

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
srcs/expansion/expand_tokens.c
srcs/expansion/expansion_vars.c
srcs/expansion/expansion_utils.c
tests/test_expansion.c
tests/test_loop.c
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
expand_tokens.c
```

Contient la fonction qui applique l'expansion sur une liste de tokens :

- expand tous les `T_WORD` ;
- skip le token juste après `T_HEREDOC` ;
- modifie les `value` en place.

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

`get_status()` est actuellement défini côté Dounia dans `srcs/main.c`.
Les tests locaux avec leur propre `main` peuvent définir une petite version
locale de `get_status` dans le fichier de test.

## Décision importante

Logique actuelle :

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
value  = "gpalemo"
after  = "!"

new_result = "hello gpalemo!"
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

État après merge avec `dev` :

```text
srcs/main.c contient get_status()
tests/test_loop.c contient une version locale de get_status pour éviter un
double main
```

## Cas testés actuellement

Les tests dans `tests/test_expansion.c` couvrent :

```text
$USER              -> gpalemo
'$USER'            -> '$USER'
"$USER"            -> "gpalemo"
abc$USER           -> abcgpalemo
"it's $USER"       -> "it's gpalemo"
'"$USER"'          -> '"$USER"'
$?                 -> 127
status:$?          -> status:127
"$?"               -> "127"
'$?'               -> '$?'
$?$USER            -> 127gpalemo
$MISSING           -> ""
a$MISSINGb         -> a
$USER$HOME         -> gpalemo/home/gpalemo
$?abc              -> 127abc
$USER?             -> gpalemo?
"$USER$?"          -> "gpalemo127"
'$USER'$HOME       -> '$USER'/home/gpalemo
$2USER             -> USER
$12USER            -> 2USER
$9abc              -> abc
$1                 -> ""
```

Les tests pipeline couvrent aussi :

```text
tokenizer -> expand_tokens
tokenizer -> expand_tokens -> remove_quotes_from_tokens
```

Cas validés :

```text
echo "$USER" '$USER' $? -> echo, "gpalemo", '$USER', 127
cat << "$USER"          -> cat, <<, "$USER"

après remove quotes :
echo "$USER" '$USER' $? -> echo, gpalemo, $USER, 127
echo '$USER'$HOME       -> echo, $USER/home/gpalemo
cat << "$USER"          -> cat, <<, $USER
```

## Commande de test

Ancienne suite expansion : à remettre à jour depuis le merge, car
`get_status()` n'est plus dans `srcs/utils/get_status.c` mais dans `srcs/main.c`.
Comme `tests/test_expansion.c` a son propre `main`, il faudra soit ajouter un
version locale de `get_status()` au test, soit déplacer `get_status()` dans un
vrai fichier utils commun.

Commande locale fiable actuellement :

```sh
make -C libft
cc -Wall -Wextra -Werror \
-Iinclude -Ilibft/inc \
tests/test_loop.c \
srcs/env/setup_env.c \
srcs/expansion/expansion.c \
srcs/expansion/expand_tokens.c \
srcs/expansion/expansion_vars.c \
srcs/expansion/expansion_utils.c \
srcs/lexer/lexer.c \
srcs/lexer/lexer_nodes.c \
srcs/lexer/lexer_redir.c \
srcs/lexer/lexer_quotes.c \
srcs/lexer/lexer_utils.c \
srcs/parser/parser.c \
srcs/parser/syntax.c \
srcs/parser/parser_utils.c \
srcs/builtins/cd.c \
srcs/exec/exec_external.c \
libft/libft.a \
-lreadline -ltermcap \
-o /tmp/test_loop
/tmp/test_loop
```

Dernier état connu : la compilation des modules passe. Attention : les tests
qui lisent `$USER`, `$HOME`, etc. dépendent de `get_env_value`.

Point observé en intégration :

```text
echo $USER -> mauvaise valeur tant que get_env_value utilise mal ft_strcmp
```

Cause signalée à Dounia :

```text
ft_strcmp(...) doit être comparé à 0 pour tester l'égalité
```

## Ce qui reste à faire

### 1. Valider les redirections dans les tests pipeline

À valider au moment de l'intégration :

```sh
echo hi > $FILE
cat < "$INPUT"
```

Les noms de fichiers doivent être expandés puis débarrassés de leurs quotes.

### 2. Mini boucle locale

`tests/test_loop.c` teste actuellement :

```text
readline
add_history
tokenizer
expand_tokens
remove_quotes_from_tokens
validate_syntax
parse_tokens
print tokens + cmd_and_args
```

État actuel :

```text
la pipeline locale est branchée jusqu'au parser
```

Prochaine étape côté parser :

```text
parser les pipes vers plusieurs t_cmd
```

### 3. Parser vers `t_cmd` / `cmd_and_args`

En place pour une commande simple, avec skip des redirections dans
`cmd_and_args`.

Objectif :

```text
tokens après expansion + remove quotes
-> t_cmd
-> cmd_and_args
-> redirections attachées à la bonne commande
```

Déjà validé :

```text
echo hi > out      -> ["echo", "hi", NULL]
cat < infile       -> ["cat", NULL]
echo hi >> log     -> ["echo", "hi", NULL]
cat << EOF         -> ["cat", NULL]
```

À clarifier avec Dounia :

- structure exacte attendue par l'exec ;
- ownership mémoire des tableaux et strings ;
- format des redirections.

### 4. Heredoc

À traiter séparément.

Règle :

```sh
cat << EOF      # contenu heredoc expandé
cat << "EOF"    # contenu heredoc non expandé
```

Le delimiter doit perdre ses quotes, mais `had_quotes` doit rester disponible
pour décider si le contenu du heredoc doit être expandé.

### 5. Brancher réellement `get_status`

`$?` lit déjà `get_status`, mais il faudra que Dounia branche les écritures :

```text
commande réussie -> *get_status() = 0
commande introuvable -> *get_status() = 127
Ctrl-C -> *get_status() = 130
```

### 6. Brancher la pipeline dans la boucle globale

À faire après accord avec Dounia sur le format envoyé à l'exec.

Ordre proposé :

```text
readline
tokenizer
expand_tokens
remove_quotes_from_tokens
syntax
parser
exec
cleanup
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
