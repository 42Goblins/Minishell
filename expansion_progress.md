# Avancement expansion — 8 septembre 2026

## Objectif

Faire l'expansion avant le retrait des quotes :

```text
tokens avec quotes conservées
-> expand_tokens
-> remove_quotes_from_tokens
-> validate_syntax
-> parse_tokens
```

Pourquoi cet ordre :

```sh
'$USER'   # pas d'expansion
"$USER"   # expansion
$USER     # expansion
```

Si les quotes sont retirées trop tôt, on perd la différence entre single quotes
et double quotes.

## État actuel

Le cœur mandatory de l'expansion est fait.

Géré :

```text
$VAR
$?
$digit
variable absente -> chaîne vide
single quotes -> pas d'expansion
double quotes -> expansion
variables collées
delimiter heredoc non expandé
```

Pas géré volontairement pour l'instant :

```text
$$
$-
${VAR}
```

## Fichiers

```text
srcs/expansion/expansion.c
srcs/expansion/expand_tokens.c
srcs/expansion/expansion_vars.c
srcs/expansion/expansion_utils.c
srcs/lexer/lexer_quotes.c
```

## Fonctions importantes

`expand_word` :

```text
scanne un mot
met à jour l'état single/double quote
remplace les $ expandables
retourne une nouvelle string
```

`expand_tokens` :

```text
parcourt la liste de tokens
expand les T_WORD
skip le T_WORD juste après T_HEREDOC
remplace token->value en place
```

`is_dollar_expand` :

```text
accepte $VAR, $?, $digit
refuse $ dans single quotes
refuse $ seul
refuse $ suivi d'un caractère non supporté
```

`get_var_value` :

```text
retourne une nouvelle string allouée
$? lit *get_status()
$digit retourne vide pour le digit consommé
variable absente retourne ""
```

## Exemples

```text
$USER        -> valeur de USER
"$USER"      -> "valeur"
'$USER'      -> '$USER'
$?           -> dernier status
status:$?    -> status:127
$2USER       -> USER
$12USER      -> 2USER
$MISSING     -> ""
"$USER$?"    -> valeur + status
```

Après `remove_quotes_from_tokens` :

```text
echo "$USER" '$USER' $? -> echo, valeur, $USER, 127
cat << "$USER"          -> cat, <<, $USER
```

## Tests

`tests/test_expansion.c` couvre les cas d'expansion isolée.

`tests/test_parser.c` et `tests/test_loop.c` couvrent l'intégration :

```text
tokenizer
-> expand_tokens
-> remove_quotes_from_tokens
-> validate_syntax
-> parse_tokens
```

Fails connus :

```text
$USER
$MISSING
```

Cause actuelle : bug côté env, pas expansion/parser.

À corriger avec Dounia :

```text
get_env_value / set_env_value doivent comparer ft_strcmp(...) == 0
```

## Status

Contrat décidé :

```c
int	*get_status(void)
{
	static int	status;

	return (&status);
}
```

```text
expansion de $? lit *get_status()
exec / builtins / erreurs / signaux écrivent *get_status()
```

## Heredoc

Ce qui est prêt côté expansion :

```text
le delimiter heredoc n'est pas expandé
les quotes du delimiter sont retirées plus tard
had_quotes reste disponible
```

Ce qui reste à faire avec Dounia :

```text
lire le contenu heredoc
expander ou non les lignes selon had_quotes
brancher le fd heredoc dans fd_in
gérer Ctrl-C pendant heredoc
```
