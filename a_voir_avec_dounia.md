# À voir avec Dounia

Checklist pour préparer l'expansion et éviter que parsing et exec se marchent
dessus.

## Priorité 1 — Recherche dans l'environnement

`get_env_value` et `set_env_value` utilisent actuellement une comparaison sur
la longueur de la clé recherchée :

```c
ft_strncmp(env->key, key, ft_strlen(key))
```

Cela peut confondre `HOME` et `HOME_TEST`.

- [ ] Lui montrer le problème.
- [ ] Lui proposer une comparaison exacte avec `ft_strcmp`, maintenant présent
  dans sa Libft.
- [ ] Confirmer que Chloé utilise directement `get_env_value` pour `$VAR` au
  lieu de créer une deuxième fonction de recherche.

Contrat proposé :

```text
Dounia maintient shell->env.
Chloé lit shell->env sans le modifier.
get_env_value retourne une adresse empruntée : Chloé ne la free jamais.
Une variable absente ou sans valeur est expand en chaîne vide.
```

## Priorité 2 — Dernier exit status pour `$?`

Il n'existe pas encore de champ pour stocker le dernier statut dans `t_shell`.

- [ ] Valider l'ajout de `int last_status` dans `t_shell`.
- [ ] Confirmer que Dounia le met à jour après chaque commande, builtin, erreur
  d'exécution et `waitpid`.
- [ ] Confirmer que Chloé le lit uniquement pour expand `$?`.
- [ ] Décider la valeur initiale au lancement du shell, normalement `0`.

Contrat proposé :

```text
exec/builtins -> mettent à jour shell->last_status
expansion     -> lit shell->last_status pour $?
```

Ne pas coder définitivement `$?` avant cette décision.

## Priorité 3 — Ordre du pipeline parsing

Faire valider cet ordre :

```text
tokenizer avec quotes conservées
-> expansion selon single/double/unquoted
-> retrait des quotes
-> parser vers t_cmd
-> exec
```

- [ ] Confirmer que l'exec reçoit des arguments déjà expandés et sans quotes
  syntaxiques.
- [ ] Confirmer que les redirections sont déjà séparées des arguments.
- [ ] Confirmer que Dounia ne relit pas la ligne brute.

## Priorité 4 — Heredoc

Le token du délimiteur conserve `had_quotes` même après retrait des quotes.

- [ ] Valider que Chloé fournit le délimiteur sans quotes, avec l'information
  indiquant s'il était quoté.
- [ ] Valider que Dounia lit le heredoc et applique ou bloque l'expansion selon
  cette information.

Exemple :

```sh
cat << EOF
cat << "EOF"
```

Le second cas ne doit pas expand les variables dans le contenu du heredoc.

## Points à fiabiliser plus tard côté environnement/exec

Ces points ne bloquent pas immédiatement `$VAR`, mais il faut les noter :

- [ ] `setup_env` retourne actuellement `void` et ne signale pas clairement un
  échec d'allocation.
- [ ] En cas d'échec au milieu de `setup_env`, la liste partielle doit être
  libérée.
- [ ] `exec_exit` appelle encore directement `exit()` sans cleanup global.
- [ ] Les retours de `exit`, des builtins et de `waitpid` doivent alimenter
  `last_status`.
- [ ] Il faudra une fonction qui libère toute la liste `t_env`, pas seulement
  un seul nœud.

## Git et fichiers générés

La nouvelle Libft contient des fichiers `libft/obj/*.o` suivis par Git.

- [ ] Lui signaler que les `.o` devraient normalement être dans `.gitignore`.
- [ ] Ne pas supprimer ces fichiers uniquement sur `chloe` sans coordination.

Le `.gitignore` ignore actuellement `libft/objs/`, mais le nouveau Makefile
utilise `libft/obj/` sans `s`.

Faire le nettoyage depuis une branche créée à partir de `dev` :

```sh
git switch dev
git pull --ff-only
git switch -c chore/repo-cleanup
```

Ajouter cette ligne dans `.gitignore` :

```gitignore
libft/obj/
```

Retirer ensuite les objets du suivi Git sans supprimer les copies locales :

```sh
git rm -r --cached libft/obj
git add .gitignore
git commit -m "chore: stop tracking generated object files"
git push -u origin chore/repo-cleanup
```

- [ ] Ouvrir une PR de `chore/repo-cleanup` vers `dev`.
- [ ] Vérifier après la PR qu'un `make` recrée les `.o` sans les faire
  apparaître dans `git status`.
- [ ] Ne pas faire ce nettoyage directement sur `main` : les suppressions déjà
  présentes sur `dev` arriveront avec la PR finale `dev -> main`.

## Répartition confirmée

```text
Chloé  : lexer, expansion, retrait des quotes, parser, erreurs de syntaxe.
Dounia : environnement, builtins, exec, wait/status, application des fd,
         lecture du heredoc et signaux.
```

Décisions communes : `t_shell`, `last_status`, contrat `t_cmd`, heredoc et
fonctions de cleanup global.
