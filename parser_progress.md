# Avancement parser — 8 septembre 2026

## Objectif

Le parser reçoit des tokens déjà préparés et fabrique une liste de `t_cmd`
utilisable par l'exec.

Pipeline actuelle :

```text
tokenizer
-> expand_tokens
-> remove_quotes_from_tokens
-> validate_syntax
-> parse_tokens
-> t_cmd / cmd_and_args / fd_in / fd_out
```

Le parser ne relit jamais la ligne brute.

## État actuel

Déjà en place :

```text
parse_tokens
create_cmd_node
create_cmd_and_args
count_cmd_args
validate_syntax
is_redirection_token
open_redirections
free_cmds
```

Ce qui marche :

```text
les pipes créent plusieurs t_cmd chaînés avec next
cmd_and_args contient seulement la commande et ses vrais arguments
les redirections et leurs targets sont retirées de cmd_and_args
< ouvre fd_in
> ouvre fd_out en truncate
>> ouvre fd_out en append
la dernière redirection du même côté gagne
les anciens fd remplacés sont fermés
free_cmds ferme les fd restants au cleanup
is_builtin est rempli avec check_is_builtins
```

Exemples :

```text
echo hello              -> ["echo", "hello", NULL]
echo hi > out           -> ["echo", "hi", NULL], fd_out vers out
cat < infile            -> ["cat", NULL], fd_in depuis infile
echo hi >> log          -> ["echo", "hi", NULL], fd_out append
echo hi > a > b         -> fd_out final vers b
echo hi | wc -c         -> cmd1 ["echo", "hi"], cmd2 ["wc", "-c"]
cat < in | grep h > out -> cmd1 fd_in, cmd2 fd_out
```

## Syntax errors

`validate_syntax` retourne :

```text
0 si OK
1 si erreur
```

Maintenant elle affiche aussi sur `stderr` et met :

```text
*get_status() = 2
```

Format :

```text
minishell: syntax error near unexpected token `TOKEN'
```

Cas validés :

```text
| echo     -> |
echo |     -> newline
echo || wc -> |
echo >     -> newline
echo <     -> newline
echo >>    -> newline
echo <<    -> newline
echo > |   -> |
echo < >   -> >
```

La mini loop de test confirme que si `validate_syntax` échoue,
`parse_tokens` n'est pas appelé.

## Redirections classiques

Contrat actuel :

```text
parser ouvre les fichiers classiques
parser remplit cmd->fd_in / cmd->fd_out
exec utilise les fd déjà prêts
```

Open fail :

```text
perror("minishell: filename")
*get_status() = 1
parse_tokens retourne NULL
la commande ne doit pas partir à l'exec
```

Cas testés :

```text
cat < fichier manquant
cat < fichier manquant | wc -l
echo hi > /tmp
echo hi > ok > /tmp
```

Tous ces cas stoppent bien le parser avec status `1`.

## Quotes et pipes

Cas important testé :

```sh
cat text.txt | "echo patate2 | wc -l"
```

Résultat attendu :

```text
cmd 0: ["cat", "text.txt", NULL]
cmd 1: ["echo patate2 | wc -l", NULL]
```

Le pipe dans les quotes reste du texte, il ne coupe pas la commande.

Cas avec quote non fermée :

```sh
cat text.txt | "echo patate2 | wc -l
```

Le tokenizer bloque avant parser.

## Tests

Dernier résultat :

```text
test_syntax : valid/invalid PASS, messages syntax sur stderr
test_parser : parser/pipes/redirs/open fail PASS
test_loop   : syntax error bloque parse_tokens, status 2
```

Fails connus dans `test_parser` :

```text
quotes + expansion
missing variable
```

Cause : bug connu dans `get_env_value`, pas parser.

## Reste à faire sans heredoc

Ordre conseillé :

```text
1. intégrer et tester avec le vrai main/exec de Dounia
2. vérifier que validate_syntax bloque bien exec dans la vraie boucle
3. vérifier que parse_tokens NULL bloque bien exec après open fail
4. vérifier fd_in/fd_out avec exec externe simple
5. vérifier fd_in/fd_out avec builtins seuls
6. vérifier fd_in/fd_out avec pipelines
7. norme + cleanup des fonctions longues/prototypes/comments
```

## Heredoc

Pas fini.

Ce qui est prêt côté lexer/expansion :

```text
T_HEREDOC existe
le delimiter n'est pas expandé
les quotes du delimiter sont retirées après expansion
had_quotes reste disponible
```

À décider/faire avec Dounia :

```text
qui lit le heredoc
qui crée le fd heredoc
qui expand ou non le contenu selon had_quotes
comment gérer Ctrl-C dans heredoc
```
