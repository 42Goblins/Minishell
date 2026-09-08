# Contrat avec Dounia

Dernière mise à jour : 8 septembre 2026.

## Répartition

```text
Moi    : lexer, tokenisation, expansion, retrait des quotes, syntaxe, parser.
Dounia : env, builtins, exec, wait/status, signaux, heredoc.
```

Ma partie prépare les données. L'exec consomme une structure déjà prête.

## Pipeline commune

Ordre à garder :

```text
line = readline(prompt)
si EOF -> exit propre
si line non vide -> add_history
tokenizer(line, shell)
expand_tokens(shell->token, shell->env)
remove_quotes_from_tokens(shell->token)
validate_syntax(shell->token)
parse_tokens(shell->token)
launch_exec(shell, shell->cmds)
cleanup tokens/cmds/line
```

Règles importantes :

```text
ne pas remove quotes avant expansion
ne pas parser si syntax error
ne pas exec si parse_tokens retourne NULL
ne pas relire la ligne brute dans l'exec
```

## Sortie du parser

Le parser donne à l'exec une liste de `t_cmd`.

Chaque node contient :

```text
cmd_and_args
fd_in
fd_out
is_builtin
access_check
path
next
```

Contrat :

```text
cmd_and_args = commande + arguments seulement
les redirections ne sont pas dans cmd_and_args
les filenames de redirection ne sont pas dans cmd_and_args
les pipes sont représentés avec cmd->next
```

## Redirections classiques

Décision confirmée : ma partie ouvre les redirections classiques.

```text
<  -> open O_RDONLY, remplit fd_in
>  -> open O_WRONLY | O_CREAT | O_TRUNC, remplit fd_out
>> -> open O_WRONLY | O_CREAT | O_APPEND, remplit fd_out
```

Si plusieurs redirections du même côté existent :

```text
la dernière gagne
l'ancien fd est fermé avant remplacement
```

Open fail :

```text
perror avec préfixe minishell
*get_status() = 1
parse_tokens retourne NULL
exec ne doit pas être appelé
```

À vérifier côté exec :

```text
exec doit utiliser cmd->fd_in et cmd->fd_out
exec ne doit pas rouvrir les fichiers depuis les tokens
single external doit appliquer fd_in/fd_out
builtin seul doit sauver/restaurer stdin/stdout si fd_in/fd_out != 0/1
pipeline doit appliquer fd_in/fd_out dans chaque child
```

## Close / ownership

Contrat actuel :

```text
open_redirections ferme seulement les fd remplacés pendant le parsing.
free_cmds ferme les fd encore stockés dans t_cmd au cleanup.
Dans un pipeline, close dans le child après dup2 ne ferme pas le fd du parent.
Le parent doit donc encore cleanup les fd restants.
```

Attention builtin dans le parent :

```text
si l'exec ferme cmd->fd_in ou cmd->fd_out dans le parent,
elle doit remettre fd_in = 0 ou fd_out = 1
sinon free_cmds peut retenter de close le même fd.
```

## Syntax errors

Ma partie gère maintenant les erreurs syntax avant parser.

```text
validate_syntax retourne 1
message sur stderr
*get_status() = 2
parse_tokens ne doit pas être appelé
exec ne doit pas être appelé
```

Format actuel :

```text
minishell: syntax error near unexpected token `TOKEN'
```

Cas gérés :

```text
| echo
echo |
echo || wc
echo >
echo > |
echo < >
```

## Expansion / env

Ma partie lit `shell->env`, elle ne le modifie pas.

Contrat mémoire :

```text
get_env_value retourne une adresse empruntée
get_var_value retourne une nouvelle string allouée
```

Bug à corriger côté env :

```text
get_env_value / set_env_value doivent tester ft_strcmp(...) == 0
```

Sans ça :

```text
$USER peut retourner HOME ou une autre variable
$MISSING peut retourner une valeur au lieu de ""
```

## Status

Décision :

```c
int	*get_status(void)
{
	static int	status;

	return (&status);
}
```

Contrat :

```text
expansion de $? lit *get_status()
exec / builtins / erreurs / signaux écrivent *get_status()
```

Ne pas ajouter en parallèle un `shell->last_status` sans rediscuter.

## Heredoc

Pas encore terminé.

Déjà prêt côté lexer/expansion :

```text
T_HEREDOC existe
le delimiter n'est pas expandé
les quotes du delimiter sont retirées
had_quotes reste disponible sur le token
```

À décider ensemble :

```text
qui lit le contenu du heredoc
qui crée le fd heredoc
où stocker ce fd
qui expand le contenu quand delimiter non quoté
comment gérer Ctrl-C dans heredoc
```

Règle bash à garder :

```text
cat << EOF    -> contenu expandé
cat << "EOF"  -> contenu non expandé
```
