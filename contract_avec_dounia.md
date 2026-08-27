# Contrat avec Dounia

Ce document fixe les décisions communes entre la partie parsing/expansion de
Chloé et la partie env/builtins/exec de Dounia.

## Répartition confirmée

```text
Chloé  : lexer, tokenisation, expansion, retrait des quotes, syntaxe, parser.
Dounia : environnement, builtins, exec, wait/status, fd, heredoc, signaux.
```

L'exec ne doit pas relire la ligne brute. Elle doit recevoir une structure déjà
préparée :

```text
arguments expandés
quotes syntaxiques retirées
redirections séparées
pipes structurés
```

## Pipeline validée

Ordre retenu :

```text
tokenizer avec quotes conservées
-> expand_tokens(tokens, env)
-> remove_quotes_from_tokens(tokens)
-> validation syntaxique
-> parser vers t_cmd
-> exec
```

État actuel au 19 août :

```text
La PR clean de Chloé est mergée dans dev.
Chloé continue sur chloe avec les docs et tests temporaires.
Le vrai main.c n'est pas encore modifié pour brancher cette pipeline.
```

Pourquoi l'expansion vient avant le retrait des quotes :

```sh
'$USER'   # pas d'expansion
"$USER"   # expansion
$USER     # expansion
```

Si les quotes étaient retirées avant l'expansion, on ne pourrait plus
distinguer single quotes et double quotes.

Contrat actuel côté Chloé :

```text
expand_tokens expand tous les tokens WORD sauf le delimiter heredoc.
remove_quotes_from_tokens passe après expand_tokens.
```

Exemples après `expand_tokens`, avant retrait des quotes :

```text
echo "$USER" '$USER' $? -> echo, "chloe", '$USER', 127
cat << "$USER"          -> cat, <<, "$USER"
```

Exemples après `remove_quotes_from_tokens` :

```text
echo "$USER" '$USER' $? -> echo, chloe, $USER, 127
cat << "$USER"          -> cat, <<, $USER
```

## À décider / faire ensemble maintenant

Objectif court terme : pouvoir donner à l'exec une structure claire, sans que
Dounia ait besoin de relire la ligne brute.

Ordre proposé pour la boucle globale :

```text
line = readline(prompt)
si EOF -> exit propre
si line non vide -> add_history
tokenizer(line, shell)
expand_tokens(shell->token, shell->env)
remove_quotes_from_tokens(shell->token)
validation syntaxique
parser tokens -> t_cmd / cmd_and_args / redirections
exec
cleanup de la commande courante
```

Ce qu'on peut brancher en local sur `chloe` avant merge :

- un wrapper de pipeline qui appelle tokenizer, expansion et retrait des quotes ;
- des tests pipeline sans toucher à l'exec de Dounia ;
- le parser vers `cmd_and_args`, tant que le contrat de sortie est clair.

Test local en cours :

```text
tests/test_loop.c
readline -> add_history -> tokenizer -> expand_tokens -> print tokens
```

Étapes suivantes dans ce test :

```text
remove_quotes_from_tokens
parse_tokens
print cmd_and_args
launch_exec seulement après validation visuelle
```

Ce qu'il vaut mieux valider avec Dounia avant merge vers `dev` :

- où `get_status()` est mis à jour après exec / builtins / erreurs / signaux ;
- quel format exact l'exec attend pour les commandes et redirections ;
- comment le heredoc reçoit le delimiter et l'information `had_quotes` ;
- qui possède/libère les `t_cmd`, `cmd_and_args` et chemins de redirection.

## Environnement et `$VAR`

Contrat :

```text
Dounia maintient shell->env.
Chloé lit shell->env sans le modifier.
```

Chloé utilise directement :

```c
get_env_value(shell->env, "HOME")
```

Elle ne crée pas une deuxième fonction de recherche dans l'environnement.

Contrat mémoire :

```text
get_env_value retourne une adresse empruntée.
Chloé ne free jamais cette adresse.
get_var_value retourne une nouvelle string allouée pour l'expansion.
```

Variable absente :

```text
$MISSING -> chaîne vide
```

Cas spéciaux déjà gérés côté expansion :

```text
$?      -> lit *get_status()
$2USER  -> USER
$12USER -> 2USER
$1      -> chaîne vide
```

On ne gère pas pour l'instant :

```text
$$      # PID shell bash
$-      # options shell bash
${VAR}  # syntaxe braces
```

## Point à corriger côté recherche env

Point observé en test loop : `echo $USER` a retourné une mauvaise valeur
d'environnement (`code.desktop` chez Chloé) alors que `$USER` vaut bien
`gpalemo` dans le terminal.

Cause probable dans `srcs/builtins/cd.c` :

```c
if (ft_strcmp(env->key, key))
```

`ft_strcmp` retourne `0` quand les strings sont égales. Cette condition matche
donc les clés différentes.

À corriger avec Dounia dans `get_env_value` et `set_env_value` :

```text
utiliser ft_strcmp(env->key, key) == 0
```

Ne pas faire non plus une comparaison partielle.

Version à éviter :

```c
ft_strncmp(env->key, key, ft_strlen(key))
```

Risque :

```text
HOME peut matcher HOME_TEST
```

La nouvelle Libft contient maintenant `ft_strcmp`.

## Dernier status et `$?`

Décision mise à jour : ne pas ajouter `last_status` dans `t_shell` pour
l'instant.

On part plutôt sur une source unique :

```c
int	*get_status(void)
{
	static int	status;

	return (&status);
}
```

Contrat :

```text
exec / builtins / erreurs / signaux -> écrivent *get_status()
expansion de $?                    -> lit *get_status()
```

Valeur initiale souhaitée :

```text
0
```

Pourquoi `get_status()` :

- accessible depuis les signaux sans passer `t_shell` ;
- évite de faire circuler `t_shell` partout ;
- proche de la logique observée chez Nico ;
- une seule source de vérité si tout le monde l'utilise.

Important :

```text
ne pas mélanger get_status() et shell->last_status
```

Après merge avec `dev`, `get_status()` existe actuellement dans `srcs/main.c`
côté Dounia, et `$?` le lit déjà dans l'expansion.

Pour les tests locaux qui ont leur propre `main`, utiliser un stub local
`get_status()` dans le fichier de test plutôt que de compiler `srcs/main.c`.

Il reste à brancher les écritures côté exec / builtins / erreurs / signaux.

## Heredoc

Le token du délimiteur conserve `had_quotes` même après retrait des quotes.

Contrat retenu :

```text
Le token après T_HEREDOC n'est pas expandé par expand_tokens.
Le délimiteur passe quand même dans remove_quotes_from_tokens.
Chloé fournit donc le délimiteur sans quotes.
Chloé conserve had_quotes sur le token.
Dounia utilise had_quotes pour décider si le contenu heredoc doit être expandé.
```

Exemples :

```sh
cat << EOF
cat << "EOF"
```

Dans le second cas, les variables dans le contenu du heredoc ne doivent pas être
expandées.

Exemple :

```text
cat << "$USER"
```

Après `expand_tokens` :

```text
cat, <<, "$USER"
```

Après `remove_quotes_from_tokens` :

```text
cat, <<, $USER
```

Le delimiter n'a pas été expandé, mais ses quotes ont été retirées.

## Points à fiabiliser plus tard côté environnement/exec

Ces points ne bloquent pas l'expansion classique `$VAR`, mais doivent être
gardés en tête :

- `setup_env` retourne actuellement `void` et ne signale pas clairement un
  échec d'allocation.
- En cas d'échec au milieu de `setup_env`, la liste partielle doit être libérée.
- `exec_exit` appelle encore directement `exit()` sans cleanup global.
- Les retours de `exit`, builtins, erreurs d'exec, `waitpid` et signaux doivent
  alimenter `get_status()`.
- Il faudra une fonction de cleanup global du shell.
- Il faudra une fonction qui libère toute la liste `t_env`, pas seulement un
  nœud.

## Git et fichiers générés

La nouvelle Libft contient des fichiers `libft/obj/*.o` suivis par Git.

Le `.gitignore` ignore actuellement `libft/objs/`, mais le nouveau Makefile
utilise `libft/obj/`.

Décision proposée :

- ne pas nettoyer ces fichiers directement sur `chloe` sans coordination ;
- faire une branche dédiée depuis `dev`.

Plan proposé :

```sh
git switch dev
git pull --ff-only
git switch -c chore/repo-cleanup
```

Ajouter dans `.gitignore` :

```gitignore
libft/obj/
```

Retirer les objets du suivi Git sans supprimer les copies locales :

```sh
git rm -r --cached libft/obj
git add .gitignore
git commit -m "chore: stop tracking generated object files"
git push -u origin chore/repo-cleanup
```

Puis ouvrir une PR vers `dev`.

## Résumé des décisions déjà prises

```text
$VAR lit shell->env via get_env_value.
get_env_value retourne une adresse empruntée.
get_var_value retourne une string allouée.
$? utilisera get_status(), pas shell->last_status.
$digit vaut vide pour le digit, le reste du mot est conservé.
expand_tokens précède remove_quotes_from_tokens.
Le delimiter heredoc n'est pas expandé comme un WORD normal.
L'exec reçoit des arguments déjà expandés et sans quotes syntaxiques.
had_quotes reste utile après retrait des quotes, surtout pour heredoc.
```
