# HANDOFF — reprise Minishell sur le portable

Dernière mise à jour : 9 août 2026, branche `chloe`.

## 1. Contexte et manière de travailler

Projet Minishell de 42, mandatory uniquement.

- Chloé : lexer/tokenisation, expansion, retrait des quotes, syntaxe et parser.
- Dounia : environnement, builtins et exec.
- Référence logique : branche `niko` de
  <https://github.com/Jdavid765/minishell/tree/niko>.
- Ne pas copier Nico aveuglément : garder ses contrats et sa logique quand ils
  conviennent, mais expliquer les adaptations.
- Chloé préfère que `i` reste un simple index, pas un `int *`.
- Utiliser `len`, pas `length`, pour les noms de variables et fonctions.
- Avancer linéairement par très petites étapes pédagogiques.
- Ne pas donner toute une fonction d'un coup, sauf demande explicite.
- Laisser les commentaires temporaires utiles à Chloé. Signaler la Norm sans
  les supprimer automatiquement.
- Les petits commentaires de travail peuvent être en français. Les `@brief`
  officiels restent courts et en anglais.
- Ne pas modifier le code de Dounia sans le dire et sans nécessité.

Documents officiels disponibles localement :

```text
/home/gpalemo/en.subject.pdf
/home/gpalemo/en.norm.pdf
```

## 2. État Git au moment du handoff

```text
branche : chloe
HEAD    : 4037735 docs: add integration checklist for expansion
remote  : origin/chloe au même commit
```

Le travail de cette session est encore non commité :

```text
M  .gitignore
M  a_voir_avec_dounia.md
M  include/minishell.h
M  srcs/lexer/lexer.c
M  srcs/lexer/lexer_handlers.c
M  srcs/lexer/lexer_quotes.c
M  tokenizer_progress.md
?? expansion_progress.md
?? srcs/expansion/
?? tests/test_expansion.c
```

Important pour passer au portable : les fichiers non commités ne seront pas
récupérables avec un simple `git pull`. Avant de changer de machine, faire un
commit et un push sur `chloe`, après un dernier `git diff` et les tests.

Sur le portable :

```sh
cd /chemin/du/minishell
git fetch --prune origin
git switch chloe
git pull --ff-only origin chloe
git status
```

Ne pas merger vers `dev` tant que le travail n'est pas propre et sélectionné.
Les tests et documents temporaires peuvent rester sur `chloe`.

## 3. Pipeline décidée

```text
readline/input
-> tokenizer
-> expansion des variables en respectant les quotes
-> remove_quotes_from_tokens
-> validation de syntaxe
-> parser vers t_cmd
-> exec de Dounia
```

L'expansion doit précéder le retrait des quotes :

```sh
'$USER'   # pas d'expansion
"$USER"   # expansion
$USER     # expansion
```

Si on supprimait d'abord les quotes, on perdrait cette information.

## 4. Lexer/tokenizer : terminé

La tokenisation mandatory est considérée terminée. Détails complets dans
`tokenizer_progress.md`.

Types reconnus :

```text
T_WORD
T_PIPE
T_REDIR_IN
T_REDIR_OUT
T_APPEND
T_HEREDOC
```

Fonctionnalités validées :

- espaces et tabulations ;
- opérateurs avec ou sans espaces autour ;
- single et double quotes ;
- quotes collées au texte dans le même mot ;
- quotes vides ;
- opérateurs protégés par les quotes ;
- détection des quotes non fermées ;
- nettoyage d'une liste partielle en cas d'échec ;
- champ `had_quotes` ;
- retrait des quotes actives.

Les quotes restent d'abord dans `token->value`. `had_quotes` reste vrai après
leur retrait, car il conserve une information historique utile au heredoc.

Les suites `|||` ou `><` sont tokenisées. Leur invalidité appartient à la future
validation de syntaxe, pas au tokenizer.

Architecture :

```text
srcs/lexer/lexer.c
srcs/lexer/lexer_handlers.c
srcs/lexer/lexer_nodes.c
srcs/lexer/lexer_redir.c
srcs/lexer/lexer_quotes.c
```

Différence assumée avec Nico : ses fonctions passent souvent `int *i`, tandis
que les handlers de Chloé retournent une longueur consommée et laissent `i`
comme index simple.

Dernière validation complète du lexer :

```text
-Wall -Wextra -Werror : OK
Norminette : tous les fichiers lexer OK
Valgrind : 121 allocations, 121 frees, 0 erreur
```

## 5. Retrait des quotes : terminé mais pas encore intégré

Dans `srcs/lexer/lexer_quotes.c` :

```c
char *remove_quotes(char *value);
int  remove_quotes_from_tokens(t_token *tokens);
```

`remove_quotes` alloue au maximum `ft_strlen(value) + 1`, puis copie sans les
quotes actives. La logique dupliquée de calcul de longueur a été supprimée.

`remove_quotes_from_tokens` remplace proprement les valeurs des `T_WORD` ayant
eu des quotes. Ne pas l'appeler dans `tokenizer` : elle sera appelée après
l'expansion.

## 6. Expansion : état exact

Détails complémentaires dans `expansion_progress.md`.

Fichiers :

```text
srcs/expansion/expansion_handlers.c
srcs/expansion/expansion.c
tests/test_expansion.c
```

Prototypes actuels :

```c
int   var_name_len(char *var);
char  *get_var_value(char *var, t_env *env);
bool  is_dollar_expand(char *word, int i, bool in_single);
char  *expand_word(char *word, t_env *env);
char  *append_expansion_part(char *built, char *part);
```

### Fonctions terminées

`var_name_len` :

- reçoit le texte juste après `$` ;
- premier caractère : lettre ou `_` ;
- suivants : alphanumériques ou `_` ;
- `USER/test` retourne `4` ;
- `2USER` retourne `0`.

`get_var_name`, static :

- appelle `var_name_len` ;
- alloue le nom avec `ft_substr` ;
- utilisée uniquement par `get_var_value`.

`get_var_value` :

- récupère le nom ;
- appelle `get_env_value` ;
- libère le nom ;
- retourne un `ft_strdup` de la valeur ;
- variable absente : retourne une chaîne vide allouée ;
- nom invalide : retourne `NULL`.

Contrat mémoire essentiel :

```text
get_env_value retourne une adresse empruntée -> ne pas la free
get_var_value retourne une nouvelle chaîne  -> le caller la free
```

`is_dollar_expand` :

- vérifie que l'index contient `$` ;
- refuse l'expansion dans les single quotes ;
- exige un nom valide après `$` ;
- ne gère pas encore `$?`.

`append_expansion_part` :

- reçoit `built`, la chaîne construite jusque-là ;
- reçoit `part`, le nouveau morceau ;
- appelle `ft_strjoin` ;
- libère toujours `built` et `part` ;
- retourne la nouvelle chaîne ou `NULL`.

Ne jamais lui passer directement des littéraux, puisqu'elle les libère :

```c
append_expansion_part(ft_strdup("hello "), ft_strdup("chloe"));
```

### `expand_word` est volontairement incomplète

Elle parcourt déjà le mot avec :

```c
int  i;
bool in_single;
bool in_double;
```

Les états de quotes sont protégés de manière croisée : une apostrophe située
dans des doubles quotes est un caractère normal, et inversement.

Le bloc détectant `is_dollar_expand` est encore vide, avec des commentaires
`// TODO` français. `expand_word` retourne encore `ft_strdup(word)` et ne
remplace donc aucune variable pour l'instant.

Les `// TODO` provoquent volontairement des erreurs Norminette. De plus,
`expand_word` dépasse actuellement 25 lignes à cause de ces commentaires. Ne
pas corriger la Norm en supprimant les repères avant que Chloé ait terminé et
compris la fonction.

## 7. Prochaine micro-étape exacte

Reprendre `expand_word` sans coder toute la fonction d'un coup.

Faire uniquement :

1. ajouter `int start` ;
2. ajouter `char *built` ;
3. après la vérification de `word`, mettre `start = 0` ;
4. faire `built = ft_strdup("")` ;
5. si l'allocation échoue, retourner `NULL` ;
6. ne pas encore remplir le bloc du `$`.

Sens :

```text
i     = caractère lu maintenant
start = début du texte pas encore copié
built = chaîne reconstruite jusque-là
```

Après cette micro-étape, expliquer avec `hello-$USER` avant d'avancer.

## 8. Tests expansion

`tests/test_expansion.c` utilise un environnement local contrôlé :

```text
USER=chloe
HOME=/home/chloe
```

Sections présentes :

- longueurs de noms ;
- valeurs récupérées ;
- décision d'expand un `$` ;
- parcours des quotes ;
- assemblage de chaînes allouées.

Les tests `[PASS]/[FAIL]` automatisés passent. Attention : la section
`QUOTE-AWARE WORD SCAN` affiche actuellement l'attendu, mais comme le `printf`
temporaire de `expand_word` a été retiré au profit des TODO, cette section ne
valide plus automatiquement le point d'expansion. Elle devra être remplacée par
des tests du vrai résultat lorsque `expand_word` sera fonctionnelle.

Compilation :

```sh
cc -Wall -Wextra -Werror \
-Iinclude -Ilibft/inc \
tests/test_expansion.c \
srcs/expansion/expansion.c \
srcs/expansion/expansion_handlers.c \
srcs/builtins/cd.c \
libft/libft.a \
-o /tmp/test_expansion

/tmp/test_expansion
```

Dernier Valgrind avant les TODO :

```text
25 allocations, 25 frees
0 byte restant
0 erreur
```

## 9. Contrat avec Dounia à confirmer

Voir aussi `a_voir_avec_dounia.md`.

### Environnement

Dounia possède et modifie `shell->env` via setup/export/unset/cd. Chloé le lit
pour l'expansion et ne libère jamais les valeurs empruntées.

`get_env_value` et `set_env_value` utilisent actuellement :

```c
ft_strncmp(env->key, key, ft_strlen(key))
```

Cela peut confondre `HOME` et `HOME_TEST`. Valider avec Dounia une comparaison
exacte, probablement `ft_strcmp`.

### `$?`

`t_shell` n'a pas encore de `last_status`. Décision souhaitée :

```text
Dounia met à jour shell->last_status après exec/builtin/erreur
Chloé le lit pour expand $?
```

Ne pas implémenter définitivement `$?` avant ce contrat.

### Heredoc

Le token délimiteur conserve `had_quotes`. Il faut confirmer que Dounia utilise
cette information pour décider si les variables du contenu du heredoc doivent
être expansées.

## 10. Robustesse finale attendue

La logique reste proche de Nico, mais il faudra faire une comparaison ciblée
avec sa branche quand expansion + parser seront terminés.

Objectif de cette comparaison : chercher les cas manquants, pas copier le code.
En particulier :

- plusieurs variables dans un même mot ;
- variables au début, milieu et fin ;
- variables absentes et valeurs vides ;
- quotes adjacentes et imbriquées selon les règles shell ;
- `$` isolé et caractères invalides après `$` ;
- redirections sans cible ;
- pipes au début, à la fin ou consécutifs ;
- combinaisons de redirections ;
- nettoyage mémoire sur toutes les erreurs.

Le tokenizer est solide, mais la majorité des cas destinés à casser un
minishell se concentrera effectivement dans l'expansion, la syntaxe, le parser,
les heredocs et leur intégration avec l'exec.
