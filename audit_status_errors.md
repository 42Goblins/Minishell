# Audit status / erreurs / messages

Ce fichier sert a noter les comportements compares avec bash avant de corriger
le code commun. L'objectif est de savoir quoi discuter avec Dounia, surtout
quand ca touche l'exec, les signaux ou la loop.

## Etat branches / base de travail

Verification faite apres `git fetch --prune origin` :

- base actuelle : branche `chloe-common-checks` creee depuis `origin/dev`
- dernier `origin/dev` vu : `44073e3 chore: keep integration notes local`
- `origin/signalv2_mdr` : rien de plus que `dev` a recuperer
- `origin/signals` : contient 2 commits non mergés, mais son contenu est plus
  vieux sur `main/parser/header` que `dev`
- `origin/dounia`, `origin/set_env`, `origin/exec_and_morev3` : anciennes
  branches de travail, pas une meilleure base que `dev` pour l'audit actuel

Conclusion : pour continuer sans refaire un bug deja corrige ailleurs, la base
la plus fiable est `origin/dev`.

## Deja valide

### Redirections

| Test | Minishell | Bash-like attendu | Etat |
| --- | --- | --- | --- |
| `cat < missing_file` puis `echo $?` | message `No such file or directory`, status `1` | status `1` | OK |
| `echo hi > /root/nope` puis `echo $?` | message `Permission denied`, status `1` | status `1` | OK |
| `cat < missing_file > /tmp/should_not_exist` | erreur sur `missing_file`, status `1`, fichier de sortie non cree | ne pas creer la sortie si l'input echoue | OK |

### Syntax

| Test | Minishell | Bash-like attendu | Etat |
| --- | --- | --- | --- |
| `|` | syntax error, status `2` | syntax error, status `2` | OK |
| `cat \|` | syntax error `newline`, status `2` | syntax error, status `2` | OK |
| `\| cat` | syntax error, status `2` | syntax error, status `2` | OK |
| `cat \|\| wc` | syntax error, status `2` | minishell ne gere pas `||` en mandatory | OK |
| `cat \| \| wc` | syntax error, status `2` | syntax error, status `2` | OK |

### Heredoc / expansion

| Test | Resultat | Etat |
| --- | --- | --- |
| `cat << EOF` avec contenu simple | contenu transmis a `cat` | OK |
| heredoc non quote avec `$USER` | expansion faite | OK |
| heredoc quote avec `$USER` | `$USER` reste litteral | OK |
| heredoc + pipe | fonctionne | OK |
| ordre `< file << EOF` | heredoc gagne | OK |
| ordre `<< EOF < file` | fichier gagne | OK |
| Ctrl-D pendant heredoc | warning + contenu deja tape envoye | OK |
| Ctrl-C pendant heredoc | retour prompt, status `130` | OK |

### Expansion

| Test | Resultat | Etat |
| --- | --- | --- |
| `echo $?` | expand le status | OK |
| `echo "$?"` | expand le status | OK |
| `echo '$?'` | reste litteral | OK |
| `echo $UNSET` | vide | OK |
| `echo "$UNSET"` | vide | OK |
| `echo a$USER b` | concatene correctement | OK |
| `echo "$USER'$USER'"` | expand dans double quotes | OK |
| `echo $2USER` | donne `USER` | OK selon choix actuel |

## Ecarts trouves cote exec

Ces points touchent plutot `srcs/exec/exec_external.c` et
`srcs/exec/exec_external_path.c`. A voir avec Dounia avant correction.

### Chemin absolu inexistant

Test :

```sh
/does/not/exist
echo $?
```

Ancien comportement minishell :

```txt
minishell: /does/not/exist: command not found
127
```

Bash :

```txt
/does/not/exist: No such file or directory
127
```

Corrige maintenant :

```txt
minishell: /does/not/exist: No such file or directory
127
```

Etat : OK.

### Execution d'un dossier

Test :

```sh
mkdir -p /tmp/ms_dir
/tmp/ms_dir
echo $?
```

Ancien comportement minishell :

```txt
execve: Permission denied
126
```

Bash :

```txt
/tmp/ms_dir: Is a directory
126
```

Corrige maintenant :

```txt
minishell: /tmp/ms_dir: Is a directory
126
```

Etat : OK.

### `..`

Test :

```sh
..
echo $?
```

Ancien comportement minishell :

```txt
execve: Permission denied
126
```

Bash :

```txt
..: command not found
127
```

Corrige maintenant :

```txt
minishell: ..: command not found
127
```

Etat : OK. Cause confirmee : `find_path` pouvait trouver un dossier via
`access(path, X_OK)`, car `X_OK` sur un dossier signifie qu'on peut le
traverser. `try_path` utilise maintenant `stat` pour refuser les dossiers avant
de retourner un chemin trouve dans `PATH`.

### Fichier existant sans permission d'execution

Test :

```sh
printf 'echo hi\n' > /tmp/ms_noexec
chmod 644 /tmp/ms_noexec
/tmp/ms_noexec
echo $?
```

Ancien comportement minishell :

```txt
minishell: /tmp/ms_noexec: command not found
127
```

Bash :

```txt
/tmp/ms_noexec: Permission denied
126
```

Corrige maintenant :

```txt
minishell: /tmp/ms_noexec: Permission denied
126
```

Etat : OK. Si le chemin contient `/` et existe, ce n'est pas un
`command not found` : le shell distingue maintenant le probleme exact du
fichier.

### `.`

Test :

```sh
.
echo $?
```

Actuel minishell :

```txt
minishell: .: command not found
127
```

Bash :

```txt
.: filename argument required
2
```

Cas special bash : `.` est un builtin bash (`source`). Pas prioritaire mandatory.
Le comportement actuel est coherent avec minishell si on ne gere pas le builtin
special `source`.

### `PATH` unset

Test :

```sh
env -u PATH ./minishell
ls
echo $?
/bin/ls >/tmp/ms_ls_out
echo $?
```

Actuel minishell :

```txt
minishell: ls: command not found
127
0
```

Bash teste dans le meme contexte trouve encore `ls` via son chemin par defaut,
mais ce comportement depend de bash et n'est pas forcement mandatory pour
minishell. A discuter avant de corriger.

### `env -i`

Test :

```sh
env -i ./minishell
env
ls
/bin/ls >/tmp/ms_env_i_ls
```

Actuel minishell :

```txt
PWD=/home/gpalemo/Minishell
SHLVL=1
PATH=/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin
ls fonctionne
/bin/ls fonctionne
```

Etat : OK avec votre setup actuel. Le shell reconstruit un env minimal avec un
`PATH`, donc les commandes externes restent utilisables.

### Chemins executables explicites

Tests :

```sh
printf '#!/bin/sh\necho REL_OK\n' > /tmp/ms_rel_exec
chmod 755 /tmp/ms_rel_exec
/tmp/ms_rel_exec
echo $?

printf '#!/bin/sh\necho LOCAL_OK\n' > ./ms_local_exec
chmod 755 ./ms_local_exec
./ms_local_exec
echo $?
```

Actuel minishell :

```txt
REL_OK
0
LOCAL_OK
0
```

Etat : OK. Les chemins explicites executables fonctionnent apres le check
`handle_direct_path_error`.

### Points du doc Nico deja refermes

- `unset FOO` supprime bien la variable cible : condition
  `ft_strcmp(current->key, cmd[i]) == 0` OK.
- Les builtins sont compares avec `ft_strcmp` dans `exec_builtins.c`, donc pas
  de matching partiel dangereux avec `ft_strncmp`.
- Les redirections sur commande seule fonctionnent avec `set_fds`.
- Le heredoc est implemente et branche sur les fd du parser.
- Les erreurs de syntaxe bloquent bien avant le parser/exec avec status `2`.

## Builtins corriges / valides

### `cd` vers chemin inexistant

Test :

```sh
cd /does/not/exist
echo $?
```

Ancien comportement minishell :

```txt
minishell: cd:/does/not/exist: No such file or directory
0
```

Bash :

```txt
cd: /does/not/exist: No such file or directory
1
```

Corrige maintenant :

```txt
minishell: cd: /does/not/exist: No such file or directory
1
```

Etat : OK.

### `cd` vers chemin sans permission

Test :

```sh
cd /root
echo $?
```

Ancien comportement minishell :

```txt
minishell: cd:/root: Permission denied
0
```

Bash :

```txt
cd: /root: Permission denied
1
```

Corrige maintenant :

```txt
minishell: cd: /root: Permission denied
1
```

Etat : OK.

### `cd` avec trop d'arguments

Test :

```sh
cd a b
echo $?
```

Actuel minishell :

```txt
minishell: cd: too many arguments
1
```

Etat : OK.

### `cd` sans `HOME`

Test :

```sh
env -u HOME ./minishell
cd
echo $?
```

Ancien comportement minishell :

```txt
cd: HOME not set
0
```

Bash-like attendu :

```txt
cd: HOME not set
1
```

Corrige maintenant :

```txt
cd: HOME not set
1
```

Etat : OK. Le helper `go_to_home_dir` renvoie maintenant un status a `exec_cd`.

### `cd -` sans `OLDPWD`

Test :

```sh
env -u OLDPWD ./minishell
cd -
echo $?
```

Ancien comportement minishell :

```txt
cd: OLDPWD not set
0
```

Bash-like attendu :

```txt
cd: OLDPWD not set
1
```

Corrige maintenant :

```txt
cd: OLDPWD not set
1
```

Etat : OK. Meme logique : `go_to_oldpwd` renvoie maintenant un status a
`exec_cd`.

### `exit abc`

Test :

```sh
exit abc
```

Actuel minishell :

```txt
exit
minishell: exit: abc: numeric argument required
process exit status 2
```

Bash :

```txt
exit: abc: numeric argument required
process exit status 2
```

Status OK. Le `exit` imprime avant l'erreur dans minishell, ce qui peut etre
acceptable comme bash interactif affiche aussi `exit` selon le contexte.

### `exit 1 2`

Test :

```sh
exit 1 2
echo $?
```

Actuel minishell :

```txt
exit
minishell: exit: too many arguments
1
```

Bash :

```txt
exit: too many arguments
1
```

Status OK. A discuter seulement si on veut ajuster l'affichage.

### `export` invalid identifier

Test :

```sh
export 123abc=test
echo $?
```

Actuel minishell :

```txt
minishell: export: `123abc=test': not a valid identifier
1
```

Bash :

```txt
export: `123abc=test': not a valid identifier
1
```

OK.

### `unset` invalid identifier

Test :

```sh
unset 123abc
echo $?
```

Actuel minishell :

```txt
0
```

Bash teste pareil dans ce contexte et retourne aussi `0`. OK pour l'instant.

## Hypothese de correction a discuter

Pour eviter que les dossiers soient traites comme des executables trouves dans
`PATH`, on verifie maintenant le type du chemin avec `stat`.

`stat` est autorise par le sujet Minishell. Ici il sert a remplir une
`struct stat` avec les informations du chemin. Ensuite `S_ISDIR(info.st_mode)`
permet de savoir si ce chemin est un dossier.

Logique :

```txt
si le chemin existe
ET ce n'est pas un dossier
ET il est executable
=> c'est une commande valide
```

Sans cette verification, `access(path, X_OK)` peut etre vrai pour un dossier.
Sur un dossier, le droit d'execution veut dire "droit de traverser le dossier",
pas "droit de lancer le dossier comme un programme".

Pour les chemins explicites contenant `/`, on distingue maintenant :

| Cas | Message | Status |
| --- | --- | --- |
| chemin inexistant | `No such file or directory` | `127` |
| chemin est un dossier | `Is a directory` | `126` |
| fichier pas executable | `Permission denied` | `126` |
| commande introuvable dans PATH | `command not found` | `127` |

## Reste a auditer

- Signaux hors heredoc :
  - `cat` puis Ctrl-C
  - `cat` puis Ctrl-D
  - pipeline puis Ctrl-C
  - Ctrl-\ si gere
- Builtins :
  - verifier d'autres cas `cd` edge-case si besoin, mais les retours d'erreur
    principaux sont OK
- Exec / PATH :
  - `PATH` unset : a discuter, car bash a un comportement par defaut special
  - `.` : bash le traite comme builtin `source`, pas prioritaire mandatory
- Valgrind global avec exec / builtins / pipelines.

## Point d'arret

Etat avant pause :

- branche de travail : `chloe-common-checks`
- base verifiee : `origin/dev` est la branche la plus a jour utile
- corrections faites :
  - `cd` retourne maintenant `1` sur erreurs (`too many arguments`, chemin
    invalide, `HOME` unset, `OLDPWD` unset)
  - les prototypes `go_to_home_dir` / `go_to_oldpwd` sont passes en `int`
  - `exec_external` gere les chemins explicites avant `find_path`
  - `try_path` utilise `stat` pour ne plus retourner un dossier trouve dans
    `PATH`
- tests OK :
  - `make`
  - `norminette` sur les fichiers touches
  - `/does/not/exist` -> `No such file or directory`, status `127`
  - `/tmp/ms_dir` -> `Is a directory`, status `126`
  - `/tmp/ms_noexec` -> `Permission denied`, status `126`
  - `..` -> `command not found`, status `127`
  - `cd` erreurs principales -> status `1`

Reprise conseillee :

1. Corriger le segfault critique sur redirection sans commande.
2. Attaquer les signaux hors heredoc : `cat` + Ctrl-C, `cat` + Ctrl-D,
   pipeline + Ctrl-C, Ctrl-\ si besoin.
3. Faire les finitions bash/norme : `exit` sur stdout, `cd -` qui affiche le
   chemin cible, fuite fd sur `exit`, champ `cmd->path` mort, commentaires `//`
   dans le header.
4. Faire Valgrind global plus tard, quand les comportements sont stabilises.

## Audit Nico du 23 septembre

Source lue : `analyse-minishell-complete.html`.

Points importants a garder pour la suite :

- La base utile reste `chloe-common-checks`; les branches `signals` et
  `signalv2_mdr` sont notees comme des leurres sans vraie gestion de signaux.
- Beaucoup de points sont confirmes OK : exec via `PATH` / relatif / absolu,
  redirections simples, heredoc, expansion, builtins principaux, status
  classiques.
- Bug critique verifie puis corrige : redirection sans commande.
  Exemples a tester :
  - `> /tmp/x`
  - `< /tmp/in`
  - `>> /tmp/a`
  - `<< EOF`
  - `> a < b`
- Cause confirmee : commande vide, donc `cmd_and_args[0] == NULL`, puis appel
  de `check_is_builtins(NULL)` ou lancement exec sans garde.
- Correction appliquee :
  - dans le parser, initialiser `cmd->is_builtin = false`
  - appeler `check_is_builtins(cmd->cmd_and_args[0])` seulement si
    `cmd->cmd_and_args[0] != NULL`
  - dans `launch_exec`, ajouter une garde au debut si `cmds->cmd_and_args[0]`
    est `NULL`, car les redirections ont deja ete ouvertes par le parser et il
    n'y a rien a executer.
- Tests OK :
  - `> /tmp/ms_redir_only` : pas de crash, fichier cree, status `0`
  - `>> /tmp/ms_redir_append` : pas de crash, fichier cree, status `0`
  - `< /tmp/ms_missing_input` : pas de crash, status `1`
  - `> /tmp/ms_a < /tmp/ms_missing_input` : pas de crash, status `1`
  - `<< EOF` sans commande : pas de crash, status `0`
- Ensuite seulement : signaux complets.
  - parent : Ctrl-C doit afficher une nouvelle ligne, vider readline, status
    `130`
  - parent : Ctrl-\ ignore
  - enfant : restaurer SIGINT/SIGQUIT en comportement par defaut avant exec
  - pipeline/single external : si SIGQUIT tue l'enfant, afficher
    `Quit (core dumped)` si on veut coller a bash.
- Finitions non bloquantes mais utiles :
  - `exit` devrait afficher `exit\n` sur stdout, pas stderr
  - `cd -` devrait afficher le repertoire cible avant `update_env_pwd`
  - `exit` peut laisser 2 fd ouverts dans Valgrind parce qu'il quitte avant la
    restauration de `exec_single_builtins`
  - `cmd->path` semble etre du code mort
  - les commentaires `//` dans `include/minishell.h` risquent Norminette
  - commentaire faux dans `exec_pipeline.c` : `while` est autorise par la norme
- Notes non bloquantes confirmees :
  - `PATH` unset : comportement actuel acceptable pour mandatory
  - `.` : bash le traite comme `source`, hors mandatory
  - `pwd` avec arguments : OK si ignore les args comme bash
