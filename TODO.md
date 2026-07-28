# TODO — Début tokenisation et parsing

## 1. Préparer les tokens

- [ ] Relire `t_token_type` et `t_token` dans `include/minishell.h`.
- [ ] Comparer `t_token` avec celui du projet de référence.
- [ ] Ajouter seulement les champs nécessaires au lexer (`value`, `type`,
  `next`, puis les indicateurs utiles comme `space_after` et `had_quotes`).
- [ ] Vérifier avec Dounia que les changements dans le `.h` ne cassent pas sa
  partie.

## 2. Créer les fonctions de liste

- [ ] Créer le dossier `srcs/parsing` ou `srcs/lexer`.
- [ ] Écrire une fonction qui crée un token.
- [ ] Écrire une fonction qui ajoute un token à la fin de la liste.
- [ ] Écrire `free_tokens`.
- [ ] Écrire temporairement `print_tokens` pour voir le résultat du lexer.

## 3. Faire un tokenizer très simple

- [ ] Lire la ligne caractère par caractère.
- [ ] Ignorer les espaces situés entre deux éléments.
- [ ] Transformer les mots simples en tokens `T_WORD`.
- [ ] Tester sans quotes, expansion ni redirection :
  - [ ] `echo hello`
  - [ ] `ls -la`
  - [ ] `pwd`

## 4. Reconnaître les opérateurs

- [ ] Créer un token `T_PIPE` pour `|`.
- [ ] Créer un token `T_REDIR_IN` pour `<`.
- [ ] Créer un token `T_REDIR_OUT` pour `>`.
- [ ] Créer un token `T_APPEND` pour `>>`.
- [ ] Créer un token `T_HEREDOC` pour `<<`.
- [ ] Vérifier que les opérateurs sont reconnus même sans espaces :
  - [ ] `ls|wc`
  - [ ] `cat<input`
  - [ ] `echo hi>>out`

## 5. Ajouter les quotes

- [ ] Lire une single quote jusqu'à la prochaine single quote.
- [ ] Lire une double quote jusqu'à la prochaine double quote.
- [ ] Détecter une quote non fermée et afficher une erreur de syntaxe.
- [ ] Conserver l'information nécessaire pour l'expansion future.
- [ ] Gérer les morceaux adjacents comme un seul argument :
  - [ ] `echo "hello world"`
  - [ ] `echo 'hello world'`
  - [ ] `echo bon"jour"`

## 6. Avant de commencer le vrai parser

- [ ] Vérifier que chaque test produit la bonne liste de tokens.
- [ ] Vérifier toutes les allocations et tous les `free`.
- [ ] Passer la Norminette.
- [ ] Faire valider la forme finale des tokens avant de construire les
  `t_cmd` attendues par Dounia.

## Priorité immédiate

Commencer uniquement par les sections 1 et 2. Une fois la liste chaînée de
tokens fiable et affichable, passer au tokenizer simple de la section 3.
