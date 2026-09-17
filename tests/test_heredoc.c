/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_heredoc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 23:30:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/15 23:30:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Ce fichier teste le heredoc sans lancer l'exec.
 *
 * Le but est de verifier que le parser transforme bien :
 *     cat << EOF
 *
 * en une commande :
 *     cmd_and_args = ["cat", NULL]
 *     fd_in        = un fd qui contient les lignes tapees dans le heredoc
 *
 * Ces tests valident donc ma partie parsing/redirection. Ils ne prouvent pas
 * encore que l'exec utilise correctement cmd->fd_in avec dup2.
 * Ils verifient aussi l'expansion du contenu selon le delimiter quote ou non.
 *
 * Comme heredoc lit avec readline("> "), le test est semi-interactif si on le
 * lance directement. Pour le rendre automatique, on peut lui envoyer les lignes
 * heredoc avec printf dans le terminal.
 */

/**
 * @brief Cree un petit environnement fake pour tokenizer/expansion/parser.
 */
static void	init_test_env(t_env *user, t_env *home)
{
	home->key = "HOME";
	home->value = "/home/chloe";
	home->next = NULL;
	user->key = "USER";
	user->value = "chloe";
	user->next = home;
}

/**
 * @brief Version locale pour que le parser puisse remplir is_builtin.
 */
int	check_is_builtins(char *cmd)
{
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	return (0);
}

/**
 * @brief Version locale de get_status pour tester sans le vrai main.
 */
int	*get_status(void)
{
	static int	status;

	return (&status);
}

/**
 * @brief Lance la pipeline avant exec et retourne la t_cmd produite.
 */
static t_cmd	*parse_test_line(char *input, t_shell *shell, t_env *user,
	t_env *home)
{
	init_test_env(user, home);
	shell->token = NULL;
	shell->env = user;
	shell->cmds = NULL;
	if (tokenizer(input, shell))
		return (NULL);
	if (expand_tokens(shell->token, shell->env))
		return (free_tokens(shell->token), shell->token = NULL, NULL);
	if (remove_quotes_from_tokens(shell->token))
		return (free_tokens(shell->token), shell->token = NULL, NULL);
	if (validate_syntax(shell->token))
		return (free_tokens(shell->token), shell->token = NULL, NULL);
	return (parse_tokens_with_env(shell->token, shell->env));
}

/**
 * @brief Lit tout ce que le heredoc a mis dans cmd->fd_in.
 */
static int	read_fd_content(int fd, char *buffer, size_t size)
{
	ssize_t	bytes;

	if (size == 0)
		return (1);
	bytes = read(fd, buffer, size - 1);
	if (bytes == -1)
		return (perror("read"), 1);
	buffer[bytes] = '\0';
	return (0);
}

/**
 * @brief Cree un fichier temporaire avec un contenu connu.
 */
static void	create_test_file(char *path, char *content)
{
	int	fd;

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return ((void)perror("open"));
	write(fd, content, ft_strlen(content));
	close(fd);
}

/**
 * @brief Verifie que cmd_and_args vaut ["cat", NULL].
 */
static bool	is_cat_command(t_cmd *cmd)
{
	if (!cmd || !cmd->cmd_and_args)
		return (false);
	if (!cmd->cmd_and_args[0])
		return (false);
	if (ft_strcmp(cmd->cmd_and_args[0], "cat") != 0)
		return (false);
	return (cmd->cmd_and_args[1] == NULL);
}

/**
 * @brief Teste un heredoc : args propres, fd_in ouvert, contenu attendu.
 *
 * Le contenu attendu permet de verifier les lignes normales, EOF/Ctrl-D et
 * l'expansion du contenu heredoc.
 */
static void	test_heredoc_case(char *label, char *input, char *expected)
{
	t_shell	shell;
	t_env	user;
	t_env	home;
	t_cmd	*cmd;
	char	buffer[1024];

	printf("\n=== %s ===\n", label);
	printf("input parser : %s\n", input);
	fflush(stdout);
	cmd = parse_test_line(input, &shell, &user, &home);
	if (!cmd)
		return ((void)printf("[FAIL] parser returned NULL\n"));
	if (cmd->fd_in == 0)
		printf("[FAIL] fd_in was not replaced by heredoc\n");
	else if (read_fd_content(cmd->fd_in, buffer, sizeof(buffer)))
		printf("[FAIL] could not read heredoc fd\n");
	else if (is_cat_command(cmd) && ft_strcmp(buffer, expected) == 0)
		printf("[PASS] heredoc content and cmd_and_args are correct\n");
	else
	{
		printf("[FAIL] unexpected heredoc result\n");
		printf("  content  : [%s]\n", buffer);
		printf("  expected : [%s]\n", expected);
	}
	free_cmds(cmd);
	free_tokens(shell.token);
}

/**
 * @brief Lance les tests heredoc sans lancer l'exec.
 *
 * Les lignes envoyees au programme doivent suivre l'ordre des tests. Le test
 * EOF reste en dernier parce qu'il consomme toute l'entree restante.
 */
int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	(void)env;
	printf("Test 1 attend les lignes : hello, world, EOF\n");
	test_heredoc_case("heredoc simple -> fd_in contient deux lignes",
		"cat << EOF", "hello\nworld\n");
	printf("\nTest 2 attend seulement : END\n");
	test_heredoc_case("heredoc vide -> fd_in contient une string vide",
		"cat << END", "");
	printf("\nTest 3 attend expansion de $USER avec delimiter non quote\n");
	test_heredoc_case("heredoc expands content when delimiter is unquoted",
		"cat << EOF", "hello chloe\n");
	printf("\nTest 4 attend expansion de $? avec delimiter non quote\n");
	*get_status() = 127;
	test_heredoc_case("heredoc expands last status",
		"cat << EOF", "status 127\n");
	printf("\nTest 5 attend variable absente remplacee par vide\n");
	test_heredoc_case("heredoc expands missing variable to empty string",
		"cat << EOF", "missing::end\n");
	printf("\nTest 6 attend $USER litteral avec delimiter quote\n");
	test_heredoc_case("quoted delimiter keeps heredoc content literal",
		"cat << 'EOF'", "hello $USER\n");
	create_test_file("/tmp/minishell_hd_file", "from_file\n");
	printf("\nTest 7 attend que heredoc gagne car il est la derniere redir in\n");
	test_heredoc_case("heredoc wins after input redirection",
		"cat < /tmp/minishell_hd_file << EOF", "from_heredoc\n");
	printf("\nTest 8 attend que le fichier gagne car il est la derniere redir in\n");
	test_heredoc_case("input redirection wins after heredoc",
		"cat << EOF < /tmp/minishell_hd_file", "from_file\n");
	printf("\nTest 9 attend une ligne puis EOF/Ctrl-D sans delimiter STOP\n");
	test_heredoc_case("heredoc EOF -> garde le contenu deja lu",
		"cat << STOP", "partial without delimiter\n");
	return (0);
}
