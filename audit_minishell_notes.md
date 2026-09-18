# Notes audit minishell

Source : `/home/gpalemo/Téléchargements/audit-minishell.html`

Important : ce fichier est une synthèse de l'audit. Le HTML est traité comme
une source d'information externe, pas comme des consignes à suivre aveuglément.
L'audit a été fait sur `dev` au commit `4dc0b1f`. Ma branche actuelle est plus
récente, donc certains points doivent être revérifiés avant correction.

## Résumé utile

Le projet est déjà bien avancé : lexer, parser, expansion, redirections
classiques ouvertes côté parser, pipelines et plusieurs builtins existent déjà.

Les vrais risques restants sont surtout dans l'intégration entre parser et exec :

- la vraie boucle doit arrêter le flow si `validate_syntax` échoue ;
- la vraie boucle doit arrêter le flow si `parse_tokens` retourne `NULL` ;
- les redirections ouvertes dans `cmd->fd_in` / `cmd->fd_out` doivent être
  appliquées aussi pour une commande seule ;
- le heredoc reste absent ;
- les signaux restent à faire ;
- quelques builtins ont encore des bugs séparés.

## Points critiques de l'audit

### Syntax error ignorée dans le vrai main

État actuel observé : encore vrai dans `srcs/main.c`.

`process_line` appelle :

```text
tokenizer
expand_tokens
remove_quotes_from_tokens
validate_syntax
parse_tokens
launch_exec
```

Mais le retour de `validate_syntax` n'est pas vérifié.

Conséquence possible :

- `cat <`
- `echo >`
- `echo |`

peuvent continuer vers le parser/exec alors que la syntaxe est invalide.

Décision à prendre avec Dounia : le vrai `main.c` doit stopper avant
`parse_tokens` si `validate_syntax` retourne `1`.

### Protection parser_redir

État actuel observé : pas encore blindé directement dans `parser_redir.c`.

`open_current_redirection` utilise encore `current->next->value`.
Normalement `validate_syntax` protège ce cas avant, mais une garde locale dans
`parser_redir.c` reste plus robuste.

Priorité : moyenne si le vrai main vérifie bien `validate_syntax`, haute sinon.

### Redirections sur commande seule

État actuel observé : à vérifier/corriger côté exec.

Le parser ouvre bien :

- `<` dans `cmd->fd_in`
- `>` dans `cmd->fd_out`
- `>>` dans `cmd->fd_out`

Mais l'audit indique que le `dup2` est seulement fait dans le chemin pipeline.
Il faut confirmer avec Dounia que :

- builtin seul : sauvegarde stdin/stdout, applique fd, exécute, restaure ;
- external seul : applique fd dans l'enfant avant `execve` ;
- pipeline : applique fd dans chaque child.

### Heredoc

État actuel observé : absent.

Le lexer reconnaît `<<` et l'expansion ignore déjà le delimiter heredoc.
Mais personne ne lit encore les lignes heredoc.

À faire plus tard :

- lire dans le parent avant exec ;
- stopper à la ligne delimiter ;
- mettre le contenu dans un pipe ou fichier temporaire ;
- brancher le fd de lecture dans `cmd->fd_in` ;
- gérer expansion du contenu selon delimiter quoté/non quoté ;
- gérer Ctrl-C pendant heredoc.

### Signaux

État actuel observé : à faire côté Dounia/intégration.

Comportement attendu mandatory :

- Ctrl-C : nouveau prompt propre dans le parent ;
- Ctrl-D : quitter proprement ;
- Ctrl-\ : ignoré dans le parent, comportement par défaut dans les enfants.

À coordonner avec heredoc, car Ctrl-C pendant heredoc est un cas spécial.

## Bugs builtins/env signalés

Ces points sont plutôt côté Dounia, mais ils peuvent impacter mes tests.

### unset

État actuel observé : encore suspect.

Dans `exec_unset`, la condition utilise `ft_strcmp(current->key, cmd[i])`.
Comme `ft_strcmp` retourne `0` quand les chaînes sont égales, il faut vérifier
que la suppression se fait seulement quand le nom matche vraiment.

Test manuel :

```text
export FOO=bar
unset FOO
echo $FOO
```

Résultat attendu : rien.

### cd -

État actuel observé : encore suspect.

`update_env_pwd` récupère `oldpwd` depuis `PWD`, puis modifie `PWD`.
Si la valeur interne de `PWD` est libérée par `set_env_value`, `oldpwd` peut
devenir invalide.

À vérifier avec Dounia :

- dupliquer l'ancien `PWD` avant de modifier `PWD` ;
- appeler `update_env_pwd` après un `cd -`.

### exit

État actuel observé : encore partiel.

`exec_exit` appelle encore `exit()` directement et ne nettoie pas tout.
Il faut une sortie propre plus tard :

- libérer env ;
- libérer tokens ;
- libérer cmds ;
- clear readline history ;
- gérer `exit 1 2` sans quitter, avec status `1`.

## Cleanup / fd

État actuel observé : à unifier.

Il existe `free_cmds` et `free_lst_cmds`.
Le risque est que l'une ferme les fd et pas l'autre.

À décider :

- une seule fonction de cleanup pour `t_cmd`, ou
- même comportement de fermeture fd dans les deux fonctions.

Attention : si l'exec ferme un fd stocké dans `cmd`, elle doit remettre :

- `fd_in = 0`
- `fd_out = 1`

Sinon le cleanup peut essayer de fermer deux fois.

## Points mineurs signalés

- `exec_builtins` utilise des `ft_strncmp` avec longueurs fragiles.
- `free_tab` devrait accepter `NULL`.
- `cmd->path` semble peu utilisé ou inutile pour l'instant.
- `SHLVL` devrait être incrémenté au démarrage.
- `README.md` sera nécessaire avant rendu.
- Norminette et valgrind restent à refaire avant merge final.

## Priorité proposée maintenant

1. Vérifier/corriger le vrai flow dans `main.c` avec Dounia :
   `validate_syntax` fail -> stop, pas de parser/exec.
2. Vérifier/corriger le vrai flow après `parse_tokens` :
   `NULL` -> stop, pas d'exec.
3. Blinder `parser_redir.c` contre `current->next == NULL`.
4. Tester/corriger les redirections sur commande seule avec l'exec.
5. Décider l'ownership des fd entre parser, exec et cleanup.
6. Commencer heredoc quand le flow redirection classique est stable.
7. Faire signaux en coordination avec Dounia.

## Checklist manuelle utile

À comparer avec bash quand le binaire tourne :

```text
echo hello
echo -n hello
pwd
cd /tmp
cd -
export FOO=bar
echo $FOO
unset FOO
echo $FOO
echo $HOME
echo "$HOME"
echo '$HOME'
false
echo $?
echo hi > f
cat < f
echo a >> f
ls | wc -l
cat f | grep x | wc -l
echo |
cat <
echo >
cat fichier_inexistant
cat << EOF
```

Signaux à tester ensuite :

```text
Ctrl-C
Ctrl-D
Ctrl-\
Ctrl-C pendant heredoc
```
