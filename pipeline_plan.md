# Pipeline globale minishell — plan de travail

## But

Avoir une ligne claire entre ma partie et celle de Dounia.

```text
Ma partie prépare la commande.
Dounia exécute une commande déjà préparée.
```

L'exec ne doit pas refaire le lexer, l'expansion ou le retrait des quotes.

## Pipeline proposée

```text
1. readline
2. tokenizer
3. expand_tokens
4. remove_quotes_from_tokens
5. validation syntaxique
6. parser vers t_cmd
7. exec
8. cleanup de la commande courante
```

## État actuel

### État Git / branches

```text
La PR clean lexer / expansion / parser a été mergée dans dev.
La branche chloe reste ma branche atelier avec les .md et tests temporaires.
Ne pas ouvrir de PR depuis chloe telle quelle.
```

### Déjà fait de mon côté

- tokenizer mandatory ;
- redirections tokenisées : `<`, `>`, `<<`, `>>` ;
- quotes conservées dans `token->value` ;
- `had_quotes` posé sur les tokens concernés ;
- `prev` ajouté et relié dans la liste de tokens ;
- `expand_word` pour `$VAR`, `$?`, `$digit` ;
- `expand_tokens` sur les `T_WORD`, sauf delimiter heredoc ;
- `remove_quotes_from_tokens` ;
- `validate_syntax` pour pipes/redirections mal placés ;
- `is_redirection_token` helper commun parser/syntax ;
- tests unitaires expansion ;
- tests syntax dédiés ;
- tests pipeline simples :

```text
tokenizer -> expand_tokens
tokenizer -> expand_tokens -> remove_quotes_from_tokens
tokenizer -> validate_syntax
```
- parser pour commande simple avec skip des redirections dans `cmd_and_args` :

```text
tokens préparés -> t_cmd -> cmd_and_args
echo hi > out -> ["echo", "hi", NULL]
cat << EOF -> ["cat", NULL]
```
- mini boucle locale dans `tests/test_loop.c` :

```text
readline -> add_history -> tokenizer -> expand_tokens
-> remove_quotes_from_tokens -> validate_syntax -> parse_tokens
-> print tokens + cmd_and_args
```

### Pas encore branché dans le vrai minishell

- appel réel de `expand_tokens` dans la boucle principale ;
- appel réel de `remove_quotes_from_tokens` après expansion ;
- appel réel de `validate_syntax` dans la boucle principale ;
- parser complet vers plusieurs `t_cmd` avec pipes ;
- stockage/ouverture des redirections dans `t_cmd` ;
- exec avec les structures finales ;
- heredoc complet.

## Ce que je peux avancer sans bloquer Dounia

### 1. Parser tokens vers commandes

Objectif :

```text
tokens préparés
-> t_cmd
-> cmd_and_args
-> redirections
```

État actuel :

```text
commande simple OK
redirections ignorées dans cmd_and_args OK
pipes pas encore transformés en plusieurs t_cmd
redirections pas encore stockées/ouvertes dans t_cmd
```

Exemples :

```sh
echo hello
echo "$USER" '$USER'
```

Après expansion + quotes :

```text
cmd_and_args = ["echo", "gpalemo", "$USER", NULL]
```

### 2. Tests redirections côté tokens/parser

Déjà validé côté `cmd_and_args` :

```sh
echo hi > out      -> ["echo", "hi", NULL]
cat < infile       -> ["cat", NULL]
echo hi >> log     -> ["echo", "hi", NULL]
cat << EOF         -> ["cat", NULL]
```

À valider ensuite quand les filenames seront stockés ou les fd ouverts :

```sh
echo hi > $FILE
cat < "$HOME"
echo hi >> '$USER'
```

But :

```text
les filenames sont bien expandés puis quotes retirées
```

### 3. Syntax validation

Refuser avant parser / exec :

```text
|
echo |
| echo
echo >
echo <>
echo || wc
```

Cette étape est du vrai code final, pas un test temporaire. Elle protège le
parser et l'exec contre des tokens impossibles.

Convention retenue :

```text
validate_syntax retourne 0 si OK, 1 si erreur.
Les erreurs de syntaxe devront mettre *get_status() = 2 dans la vraie boucle.
Les messages doivent aller sur stderr.
```

Messages visés à terme :

```text
minishell: syntax error near unexpected token `|'
minishell: syntax error near unexpected token `newline'
```

Pour l'instant, on peut garder un affichage temporaire dans les tests, puis
centraliser les messages quand la boucle principale sera branchée.

## À faire avec Dounia

### 1. Contrat `t_cmd`

Décider ensemble le format exact :

```text
cmd_and_args
fd_in / fd_out ou liste de redirections
next pour les pipes
```

Questions :

- est-ce que ma partie ouvre les redirections ou Dounia ?
- est-ce que parser stocke juste les filenames et types ?
- qui libère `cmd_and_args` ?
- comment signaler une erreur de parsing ou d'ouverture ?
- qui affiche les erreurs finales et qui écrit `*get_status()` ?

### 2. `get_status`

Décision actuelle :

```c
int	*get_status(void)
{
	static int	status;

	return (&status);
}
```

Contrat :

```text
expansion lit *get_status()
exec / builtins / erreurs / signaux écrivent *get_status()
```

### 3. Heredoc

Contrat actuel :

```text
delimiter après T_HEREDOC non expandé par expand_tokens
delimiter passe dans remove_quotes_from_tokens
had_quotes reste disponible
```

Règle :

```sh
cat << EOF      # contenu expandé
cat << "EOF"    # contenu non expandé
```

À décider :

- qui lit le heredoc ;
- qui appelle l'expansion sur les lignes du contenu ;
- comment transmettre `had_quotes`.

### 4. Erreurs et status

À clarifier ensemble :

```text
Ma partie détecte syntax errors avant parser/exec.
Dounia gère les erreurs d'exec, builtins, waitpid, signaux.
```

Répartition proposée :

```text
Ma partie :
- quote non fermée
- pipe mal placé
- redirection sans filename
- token inattendu
- erreur malloc parser

Dounia :
- command not found -> 127
- permission denied -> 126
- open redirection fail -> 1
- builtin fail -> 1
- Ctrl-C -> 130
- Ctrl-\ -> 131
```

Point de design :

```text
Soit validate_syntax print + set status directement.
Soit validate_syntax retourne un code, et la boucle centrale print + set status.
```

Décision provisoire :

```text
validate_syntax ne modifie pas les tokens.
validate_syntax retourne 0/1.
On centralise les messages/status quand main loop sera décidée.
```

## Merge vers dev

Dernier merge réalisé :

```text
chloe-lexer-parser-pr -> dev
méthode GitHub : rebase and merge
```

Pour les prochains merges, garder la même stratégie :

```text
chloe = branche atelier avec notes et tests temporaires
branche-pr-clean = branche créée depuis origin/dev avec seulement le code à merger
```

Plan recommandé :

```text
1. commit propre sur chloe
2. push origin chloe
3. fetch origin
4. créer une branche PR clean depuis origin/dev
5. restaurer uniquement les fichiers de code depuis chloe
6. vérifier Files changed : pas de .md perso, pas de tests temporaires
7. PR vers dev
```

Le point sensible attendu : `include/minishell.h`.

## Prochaine étape recommandée

Prochaine grosse étape de mon côté :

```text
parser les pipes vers plusieurs t_cmd chaînés
```

Juste après :

```text
décider avec Dounia du contrat redirections :
- parser ouvre fd_in/fd_out
- ou parser stocke filenames/types et exec ouvre
```

Point bloquant connu avec Dounia :

```text
get_env_value / set_env_value doivent comparer avec ft_strcmp(...) == 0
```

Tant que ce bug existe, les tests `$USER`, `$HOME`, etc. peuvent sortir une
mauvaise valeur même si l'expansion est bien appelée.

Gros morceau restant après pipes + redirections :

```text
heredoc complet, à faire/valider avec Dounia
```
