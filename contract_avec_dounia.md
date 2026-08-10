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
-> expansion selon single/double/unquoted
-> retrait des quotes
-> validation syntaxique
-> parser vers t_cmd
-> exec
```

Pourquoi l'expansion vient avant le retrait des quotes :

```sh
'$USER'   # pas d'expansion
"$USER"   # expansion
$USER     # expansion
```

Si les quotes étaient retirées avant l'expansion, on ne pourrait plus
distinguer single quotes et double quotes.

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

## Point à corriger côté recherche env

À vérifier avec Dounia : `get_env_value` et `set_env_value` ne doivent pas faire
une comparaison partielle.

Version à éviter :

```c
ft_strncmp(env->key, key, ft_strlen(key))
```

Risque :

```text
HOME peut matcher HOME_TEST
```

Décision souhaitée :

```text
utiliser une comparaison exacte, probablement ft_strcmp
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

`$?` ne doit pas être codé définitivement avant que `get_status()` soit ajouté
dans le projet.

## Heredoc

Le token du délimiteur conserve `had_quotes` même après retrait des quotes.

Contrat souhaité :

```text
Chloé fournit le délimiteur sans quotes.
Chloé conserve had_quotes sur le token.
Dounia utilise had_quotes pour décider si le contenu heredoc doit être expand.
```

Exemples :

```sh
cat << EOF
cat << "EOF"
```

Dans le second cas, les variables dans le contenu du heredoc ne doivent pas être
expandées.

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
L'expansion précède le retrait des quotes.
L'exec reçoit des arguments déjà expandés et sans quotes syntaxiques.
had_quotes reste utile après retrait des quotes, surtout pour heredoc.
```
