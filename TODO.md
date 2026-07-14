# TODO Minishell

Objectif: avancer dans l'ordre, sans partir sur les bonus, et garder un contrat
simple entre parsing et execution.

Legende:

- `[ ]` a faire
- `[x]` fait
- `Chloe` parsing
- `Dounia` execution
- `Ensemble` decisions communes

## 0. Base deja lancee

### Ensemble

- [x] Repartir d'un repo propre.
- [x] Clarifier la repartition: Chloe parsing, Dounia execution.
- [x] Ecrire une premiere organisation globale dans `organisation.md`.
- [x] Decider qu'on ne fait pas les bonus au debut.

### Chloe

- [x] Ajouter les includes utiles dans `include/minishell.h`.
- [x] Ajouter les premieres structs de tokens: `t_token_type`, `t_token`.
- [x] Ajouter `new_token`.
- [x] Ajouter `add_token_back`.

### Dounia

- [ ] Lire `organisation.md`.
- [ ] Lire la section contrat v1 parsing -> execution.

## 1. Discussion prioritaire ensemble

But: avant de trop coder chacune de son cote, se mettre d'accord sur le minimum
qui ne doit pas changer toutes les deux heures.

### Ensemble

- [ ] Valider que le contrat v1 est acceptable pour toutes les deux.
- [ ] Valider les structs communes minimales: `t_token`, `t_redir`, `t_cmd`.
- [ ] Decider les noms exacts des champs principaux: `argv`, `redirs`, `next`.
- [ ] Decider qui appelle `parse_line`, `execute_pipeline`, `free_cmds`.
- [ ] Decider comment stocker `env` et `last_status`.
- [ ] Decider le format simple des erreurs de syntaxe.
- [ ] Decider comment tester l'exec si le parser n'est pas encore fini.

### Chloe

- [ ] Presenter a Dounia le flow voulu:

```text
line -> parse_line() -> t_cmd *cmds -> execute_pipeline() -> free_cmds()
```

### Dounia

- [ ] Dire ce dont elle a besoin dans `t_cmd` pour commencer l'exec.
- [ ] Dire si elle prefere tester avec des `t_cmd` creees a la main au debut.

## 2. Semaine 1 - Tokenisation simple

But: obtenir une liste de tokens affichable en debug. Pas encore de parser
complet, pas encore d'exec.

### Ensemble

- [ ] Verifier que `make` compile sur les deux machines.
- [ ] Se mettre d'accord sur les dossiers: `srcs/parsing`, `srcs/exec`,
  `srcs/builtins`.

### Chloe

- [ ] Finir les outils tokens: `free_tokens`.
- [ ] Ajouter un debug simple: `print_tokens`.
- [ ] Coder un lexer v1: mots separes par espaces.
- [ ] Ajouter les tokens speciaux: `|`, `<`, `>`, `<<`, `>>`.
- [ ] Tester avec `echo hello`, `cat < input`, `echo hi > out`.
- [ ] Noter les cas qui ne sont pas encore geres: quotes, expansion, erreurs.

### Dounia

- [ ] Lire les fonctions autorisees cote execution.
- [ ] Relire comment marchent `fork`, `execve`, `waitpid`.
- [ ] Relire comment marchent `pipe`, `dup2`, `open`.
- [ ] Commencer une fiche perso sur les builtins obligatoires.

## 3. Semaine 2 - Contrat `t_cmd` minimal

But: passer de tokens a une structure de commande assez simple pour que Dounia
puisse commencer a brancher l'exec.

### Ensemble

- [ ] Ajouter ou valider `t_redir` et `t_cmd` dans `include/minishell.h`.
- [ ] Valider que `cmd->next` represente les pipes.
- [ ] Valider que `argv` est toujours termine par `NULL`.
- [ ] Valider que les redirections ne sont jamais dans `argv`.

### Chloe

- [ ] Coder les fonctions de creation/free de `t_cmd`.
- [ ] Coder un parser v1 sans quotes ni expansion.
- [ ] Gerer une commande simple: `echo hello`.
- [ ] Gerer une redirection simple: `echo hello > out`.
- [ ] Gerer un pipe simple: `ls | wc`.
- [ ] Ajouter `print_cmds` pour debug.

### Dounia

- [ ] Coder une execution simple a partir d'un `argv`.
- [ ] Tester `/bin/ls` avec une `t_cmd` creee a la main.
- [ ] Tester `ls` avec recherche dans `PATH`.
- [ ] Commencer `echo`, `pwd`, `env`.

## 4. Semaine 3 - Quotes et builtins

But: rendre les commandes simples plus proches du vrai sujet.

### Ensemble

- [ ] Comparer les comportements avec `bash`.
- [ ] Faire une mini liste de tests communs a relancer souvent.

### Chloe

- [ ] Gerer les single quotes.
- [ ] Gerer les double quotes.
- [ ] Retirer les quotes de syntaxe avant l'exec.
- [ ] Detecter les quotes non fermees.
- [ ] Tester `echo "hello world"` et `echo '$HOME'`.

### Dounia

- [ ] Finir `echo -n`.
- [ ] Finir `pwd`.
- [ ] Finir `env`.
- [ ] Commencer `cd`.
- [ ] Commencer `export` / `unset`.

## 5. Semaine 4 - Expansion et redirections

But: connecter le parsing avec l'environnement et rendre les redirections
fonctionnelles.

### Ensemble

- [ ] Valider comment `last_status` est transmis au parsing.
- [ ] Valider comment le parsing lit l'environnement.
- [ ] Tester ensemble `echo $HOME`, `echo "$HOME"`, `echo '$HOME'`.

### Chloe

- [ ] Gerer `$VAR`.
- [ ] Gerer `$?`.
- [ ] Respecter les single quotes pour bloquer l'expansion.
- [ ] Garder l'expansion active dans les double quotes.
- [ ] Finaliser les erreurs simples de redirection: `echo >`, `cat <`.

### Dounia

- [ ] Appliquer `<` avec `open` + `dup2`.
- [ ] Appliquer `>` avec `open` + `dup2`.
- [ ] Appliquer `>>` avec `open` + `dup2`.
- [ ] Verifier les erreurs de fichiers avec `perror` ou message propre.

## 6. Semaine 5 - Pipes et integration

But: faire marcher plusieurs commandes ensemble.

### Ensemble

- [ ] Brancher `parse_line` avec `execute_pipeline`.
- [ ] Tester une commande simple de bout en bout.
- [ ] Tester un pipe de bout en bout.
- [ ] Tester une redirection de bout en bout.

### Chloe

- [ ] Verifier les erreurs de pipe: `|`, `echo |`, `echo || wc`.
- [ ] Verifier que chaque pipe cree bien une nouvelle `t_cmd`.
- [ ] Nettoyer les leaks cote parsing.

### Dounia

- [ ] Gerer un pipe simple: `ls | wc -l`.
- [ ] Gerer plusieurs pipes: `cat file | grep hello | wc -l`.
- [ ] Recuperer le bon exit status final.
- [ ] Nettoyer les fd inutiles.

## 7. Semaine 6 - Heredoc, signaux, finition

But: finir l'obligatoire et stabiliser le comportement.

### Ensemble

- [ ] Relire le sujet officiel une derniere fois.
- [ ] Faire une liste de tests de correction.
- [ ] Verifier que les bonus ne sont pas melanges au obligatoire.
- [ ] Verifier norminette.
- [ ] Verifier leaks.
- [ ] Faire le README en anglais.

### Chloe

- [ ] Parser `<< delimiter`.
- [ ] Stocker le heredoc dans `t_redir`.
- [ ] Tester les erreurs de syntaxe restantes.
- [ ] Retirer ou cacher les fonctions debug avant rendu.

### Dounia

- [ ] Lire le contenu heredoc jusqu'au delimiter.
- [ ] Brancher heredoc sur l'entree de la commande.
- [ ] Gerer `Ctrl-C`, `Ctrl-D`, `Ctrl-\`.
- [ ] Verifier les builtins qui modifient le shell dans le parent.

## 8. Tests minimum a cocher souvent

### Parsing seul

- [ ] `echo hello`
- [ ] `echo "hello world"`
- [ ] `echo '$HOME'`
- [ ] `echo "$HOME"`
- [ ] `cat < input`
- [ ] `echo hello > out`
- [ ] `ls | wc -l`

### Execution seule

- [ ] `pwd`
- [ ] `env`
- [ ] `/bin/ls`
- [ ] `ls`
- [ ] `echo hello`
- [ ] `cd ..`

### Integration

- [ ] `echo $HOME`
- [ ] `env | grep PATH`
- [ ] `echo hello > file`
- [ ] `cat < file`
- [ ] `ls | wc -l`
- [ ] `cat << EOF`

## 9. Regles pour ne pas se perdre

- [ ] Ne pas coder les bonus avant que l'obligatoire soit stable.
- [ ] Ne pas modifier les structs communes sans prevenir l'autre.
- [ ] Faire des petits commits lisibles.
- [ ] Tester avant de merge.
- [ ] Garder une branche d'integration si chacune travaille de son cote.
- [ ] Noter dans `a_lire_dounia.md` les changements importants quand l'autre est absente.
