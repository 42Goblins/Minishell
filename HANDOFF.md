# Reprise du travail sur Minishell

Ce document donne le contexte nécessaire pour reprendre le travail dans une
nouvelle conversation, sur une autre machine, sans devoir tout réexpliquer.

## 1. Contexte du projet

Projet Minishell de 42 réalisé en binôme :

- Chloé (`cmauley`) travaille sur la tokenisation, l'expansion et le parsing.
- Dounia travaille principalement sur l'environnement, les builtins et l'exec.
- Les bonus ne seront pas faits pour le moment.

Chloé travaille sur la branche `chloe`. La branche a déjà été mise à jour avec
`dev`, qui contient du travail récent de Dounia.

Ne pas supprimer ou réécrire le travail de Dounia sans nécessité. Les tests du
lexer doivent rester dans `tests/` afin de ne pas modifier son `main.c`.

## 2. Références à utiliser

Références officielles :

- sujet Minishell obligatoire : `en.subject.pdf` ;
- Norme officielle : `en.norm.pdf`.

Ces PDF se trouvaient dans `/home/gpalemo/` sur le premier ordinateur. Ils
devront être copiés sur le portable si nécessaire.

Projet de référence des amis :

<https://github.com/Jdavid765/minishell/tree/niko>

La branche `niko` sert de référence principale pour la logique de tokenisation,
d'expansion, de parsing et pour le contrat envoyé à l'exec de Dounia.

Il ne faut cependant pas copier aveuglément ce projet. Une autre solution peut
être proposée si elle est plus claire, plus pédagogique ou plus robuste, mais
la différence et sa raison doivent être expliquées à Chloé.

Les anciens fichiers Markdown du dépôt ne constituent pas une spécification du
projet. `TODO.md` a été remplacé par une feuille de route plus récente.

## 3. Manière de travailler demandée

- Avancer linéairement, par petites étapes logiques.
- Expliquer le raisonnement avant de donner du code.
- Agir comme un professeur : Chloé doit comprendre et pouvoir expliquer le
  code pendant l'évaluation.
- Ne pas donner directement toutes les lignes, sauf si Chloé le demande ou si
  elle souhaite avancer plus vite.
- Vérifier la Norminette, la compilation, les erreurs d'allocation et les
  fuites au fur et à mesure.
- Garder `i` comme un simple index entier dans le tokenizer. Chloé ne souhaite
  pas transmettre l'index avec un `int *`.
- Les commentaires `@brief` sont courts, utiles, placés au-dessus des fonctions
  et écrits en anglais conformément à la Norme.
- Ne pas modifier le code de Dounia pour corriger des problèmes sans rapport
  avec le lexer.

Ordre de priorité pour les décisions :

1. Sujet officiel.
2. Norme officielle.
3. Contrat avec l'exec de Dounia.
4. Logique du projet de Nico.
5. Adaptations pédagogiques clairement annoncées.

## 4. État actuel du tokenizer

La première version fonctionnelle reconnaît :

- les mots séparés par des espaces ;
- les mots séparés par des tabulations ;
- le pipe `|` avec ou sans espaces autour.

Exemples fonctionnels :

```sh
echo hello
echo hello | wc
echo hello|wc
```

Pour `echo hello|wc`, la liste attendue est :

```text
T_WORD("echo")
-> T_WORD("hello")
-> T_PIPE("|")
-> T_WORD("wc")
-> NULL
```

Ne sont pas encore gérés :

- `<` et `>` ;
- `<<` et `>>` ;
- les quotes simples et doubles ;
- les expansions `$VAR` et `$?` ;
- les erreurs de syntaxe ;
- la transformation des tokens vers les `t_cmd` de Dounia.

## 5. Structures et contrat mémoire

Le token minimal actuel contient :

```c
typedef struct s_token
{
	t_token_type		type;
	char			*value;
	struct s_token	*next;
}					t_token;
```

Le token devient propriétaire de la chaîne placée dans `value` :

```text
ft_substr ou ft_strdup alloue value
-> create_token_node conserve cette adresse
-> free_tokens libère value puis le token
```

Les champs présents chez Nico comme `prev`, `is_valid`, `had_quotes` et
`space_after` n'ont pas encore été ajoutés. Ils seront étudiés au moment où les
quotes et l'expansion les rendront nécessaires.

## 6. Organisation actuelle

### `srcs/lexer/lexer_nodes.c`

- `create_token_node`
- `add_token_back`
- `free_tokens`

### `srcs/lexer/lexer.c`

- `add_word_token` : crée un `T_WORD` ;
- `add_pipe_token` : crée un `T_PIPE` ;
- `add_next_token` : choisit le token à créer et renvoie sa longueur ;
- `tokenizer_error` : nettoie une liste partielle ;
- `tokenizer` : boucle principale avec `i` comme index courant.

Les fonctions de création `WORD` et `PIPE` restent ensemble dans `lexer.c`.

### `srcs/lexer/lexer_handlers.c`

- `is_blank` : reconnaît espace et tabulation ;
- `word_length` : calcule la longueur du prochain mot.

Ce fichier contient les petits helpers de lecture et de détection.

### `srcs/lexer/lexer_debug.c`

- `print_tokens` affiche temporairement le type numérique et la valeur.

### `tests/test_lexer.c`

Test isolé du tokenizer. Il utilise actuellement `echo hello|wc`, affiche la
liste puis appelle `free_tokens`.

Des explications supplémentaires se trouvent dans `tokenizer_progress.md`.

## 7. Compiler et lancer le test

Installer Readline sur Ubuntu si nécessaire :

```sh
sudo apt update
sudo apt install libreadline-dev
```

Construire la Libft si `libft/libft.a` n'existe pas :

```sh
make -C libft
```

Compiler le test :

```sh
cc -Wall -Wextra -Werror -Iinclude -Ilibft tests/test_lexer.c srcs/lexer/lexer.c srcs/lexer/lexer_handlers.c srcs/lexer/lexer_nodes.c srcs/lexer/lexer_debug.c libft/libft.a -o /tmp/test_lexer
```

Lancer le test :

```sh
/tmp/test_lexer
```

Résultat attendu :

```text
type: 0, value: echo
type: 0, value: hello
type: 1, value: |
type: 0, value: wc
```

Au dernier contrôle, les fichiers du lexer et le test passaient la Norminette.

## 8. Prochaine étape exacte

Ajouter les redirections progressivement, en commençant par les opérateurs à un
caractère :

```text
<  puis  >
```

Ensuite seulement, différencier les opérateurs à deux caractères :

```text
<<  et  >>
```

Le lexer doit les reconnaître même sans espaces :

```sh
cat<input
echo hello>output
cat<<EOF
echo hello>>output
```

Il faudra probablement généraliser la détection des séparateurs dans
`lexer_handlers.c`, tout en gardant les fonctions de création de tokens dans
`lexer.c` tant que ce fichier reste cohérent et conforme à la limite de cinq
fonctions.

## 9. Reprendre sur le portable

Après avoir cloné ou ouvert le dépôt :

```sh
git fetch --prune
git switch chloe
git pull --ff-only origin chloe
git status
```

Demande ensuite à la nouvelle conversation :

```text
Lis entièrement HANDOFF.md, tokenizer_progress.md et l'état actuel des fichiers
du lexer. Reprends à la section « Prochaine étape exacte », sans modifier le
travail de Dounia et en avançant pédagogiquement par petites étapes.
```
