# Pipeline globale minishell — plan de travail

## But

Avoir une ligne claire entre le travail de Chloé et celui de Dounia.

```text
Chloé prépare la commande.
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
La branche chloe reste la branche atelier avec les .md et tests temporaires.
Ne pas ouvrir de PR depuis chloe telle quelle.
```

### Déjà fait côté Chloé

- tokenizer mandatory ;
- redirections tokenisées : `<`, `>`, `<<`, `>>` ;
- quotes conservées dans `token->value` ;
- `had_quotes` posé sur les tokens concernés ;
- `prev` ajouté et relié dans la liste de tokens ;
- `expand_word` pour `$VAR`, `$?`, `$digit` ;
- `expand_tokens` sur les `T_WORD`, sauf delimiter heredoc ;
- `remove_quotes_from_tokens` ;
- tests unitaires expansion ;
- tests pipeline simples :

```text
tokenizer -> expand_tokens
tokenizer -> expand_tokens -> remove_quotes_from_tokens
```
- parser V1 pour commande simple :

```text
tokens préparés -> t_cmd -> cmd_and_args
```
- mini boucle locale dans `tests/test_loop.c` :

```text
readline -> add_history -> tokenizer -> expand_tokens -> print tokens
```

### Pas encore branché dans le vrai minishell

- appel réel de `expand_tokens` dans la boucle principale ;
- appel réel de `remove_quotes_from_tokens` après expansion ;
- syntax validation ;
- parser complet vers `t_cmd` / `cmd_and_args` avec pipes et redirections ;
- exec avec les structures finales ;
- heredoc complet.

## Ce que Chloé peut avancer sans bloquer Dounia

### 1. Parser tokens vers commandes

Objectif :

```text
tokens préparés
-> t_cmd
-> cmd_and_args
-> redirections
```

À faire prudemment :

- commencer par une commande simple sans pipe ;
- remplir `cmd_and_args` avec les tokens `T_WORD` ;
- ignorer ou stocker séparément les redirections ;
- ajouter les pipes ensuite.

Exemples :

```sh
echo hello
echo "$USER" '$USER'
```

Après expansion + quotes :

```text
cmd_and_args = ["echo", "chloe", "$USER", NULL]
```

### 2. Tests redirections côté tokens

Sans exec :

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

## À faire avec Dounia

### 1. Contrat `t_cmd`

Décider ensemble le format exact :

```text
cmd_and_args
fd_in / fd_out ou liste de redirections
next pour les pipes
```

Questions :

- est-ce que Chloé ouvre les redirections ou Dounia ?
- est-ce que parser stocke juste les filenames et types ?
- qui libère `cmd_and_args` ?
- comment signaler une erreur de parsing ou d'ouverture ?

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

Dans `tests/test_loop.c` :

```text
1. brancher remove_quotes_from_tokens après expand_tokens
2. afficher les tokens après retrait des quotes
3. brancher parse_tokens
4. afficher cmd_and_args
```

Ensuite seulement :

```text
tester launch_exec sur une commande externe simple
```

Point bloquant connu avec Dounia :

```text
get_env_value / set_env_value doivent comparer avec ft_strcmp(...) == 0
```

Tant que ce bug existe, les tests `$USER`, `$HOME`, etc. peuvent sortir une
mauvaise valeur même si l'expansion est bien appelée.
