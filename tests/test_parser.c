/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_parser.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 20:08:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/17 20:08:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Ce fichier teste le debut du parser sans lancer l'exec.
 *
 * Il prend une ligne, passe par tokenizer, expansion, retrait des quotes,
 * validation syntaxique, puis parse_tokens. Le but actuel est de verifier que
 * cmd_and_args contient seulement les vrais arguments de commande, sans les
 * operateurs de redirection ni leurs filenames/delimiters. Il teste aussi le
 * debut du parsing des pipes avec plusieurs t_cmd chainees.
 *
 * Les pipes et le stockage/ouverture des redirections dans t_cmd ne sont pas
 * encore finis.
 */

/**
 * @brief Cree un petit environnement fake pour tester le parser.
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
 * @brief Compare le tableau cmd_and_args avec les strings attendues.
 */
static bool	args_match(char **args, char **expected)
{
	int	i;

	i = 0;
	while (args && expected && args[i] && expected[i])
	{
		if (ft_strcmp(args[i], expected[i]) != 0)
			return (false);
		i++;
	}
	return (args && expected && args[i] == NULL && expected[i] == NULL);
}

/**
 * @brief Affiche un tableau de strings sous forme ["a", "b", NULL].
 */
static void	print_args(char **args)
{
	int	i;

	i = 0;
	printf("[");
	while (args && args[i])
	{
		if (i > 0)
			printf(", ");
		printf("\"%s\"", args[i]);
		i++;
	}
	if (i > 0)
		printf(", ");
	printf("NULL]");
}

/**
 * @brief Cree un petit fichier temporaire pour tester les redirections input.
 */
static void	create_test_file(char *path)
{
	int	fd;

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return ;
	write(fd, "hello\n", 6);
	close(fd);
}

/**
 * @brief Lance le pipeline parser et compare cmd_and_args au resultat attendu.
 */
static void	test_cmd_args_case(char *label, char *input, char **expected)
{
	t_shell	shell;
	t_env	user;
	t_env	home;
	t_cmd	*cmd;

	init_test_env(&user, &home);
	shell.token = NULL;
	shell.env = &user;
	shell.cmds = NULL;
	if (tokenizer(input, &shell))
		return ((void)printf("[FAIL] tokenizer: %s\n", label));
	if (expand_tokens(shell.token, shell.env))
		return (free_tokens(shell.token), (void)printf("[FAIL] expansion: %s\n",
				label));
	if (remove_quotes_from_tokens(shell.token))
		return (free_tokens(shell.token), (void)printf("[FAIL] quotes: %s\n",
				label));
	if (validate_syntax(shell.token))
		return (free_tokens(shell.token), (void)printf("[FAIL] syntax: %s\n",
				label));
	cmd = parse_tokens(shell.token);
	if (!cmd)
		return (free_tokens(shell.token), (void)printf("[FAIL] parser: %s\n",
				label));
	if (args_match(cmd->cmd_and_args, expected))
		printf("[PASS] %s\n", label);
	else
		printf("[FAIL] %s\n", label);
	printf("  input    : %s\n", input);
	printf("  result   : ");
	print_args(cmd->cmd_and_args);
	printf("\n  expected : ");
	print_args(expected);
	printf("\n");
	free_cmds(cmd);
	free_tokens(shell.token);
}

/**
 * @brief Compare les commandes chainees avec les tableaux attendus.
 */
static bool	cmds_match(t_cmd *cmds, char ***expected)
{
	int	i;

	i = 0;
	while (cmds && expected[i])
	{
		if (!args_match(cmds->cmd_and_args, expected[i]))
			return (false);
		cmds = cmds->next;
		i++;
	}
	return (cmds == NULL && expected[i] == NULL);
}

/**
 * @brief Affiche chaque commande chainee avec son resultat attendu.
 */
static void	print_cmds_result(t_cmd *cmds, char ***expected)
{
	int	i;

	i = 0;
	while (cmds && expected[i])
	{
		printf("  cmd %d    : ", i);
		print_args(cmds->cmd_and_args);
		printf("\n  expected : ");
		print_args(expected[i]);
		printf("\n");
		cmds = cmds->next;
		i++;
	}
}

/**
 * @brief Lance le pipeline parser et verifie une liste de commandes chainees.
 */
static void	test_pipe_case(char *label, char *input, char ***expected)
{
	t_shell	shell;
	t_env	user;
	t_env	home;
	t_cmd	*cmds;

	init_test_env(&user, &home);
	shell.token = NULL;
	shell.env = &user;
	shell.cmds = NULL;
	if (tokenizer(input, &shell))
		return ((void)printf("[FAIL] pipe tokenizer: %s\n", label));
	if (expand_tokens(shell.token, shell.env))
		return (free_tokens(shell.token),
			(void)printf("[FAIL] pipe expansion: %s\n", label));
	if (remove_quotes_from_tokens(shell.token))
		return (free_tokens(shell.token),
			(void)printf("[FAIL] pipe quotes: %s\n", label));
	if (validate_syntax(shell.token))
		return (free_tokens(shell.token),
			(void)printf("[FAIL] pipe syntax: %s\n", label));
	cmds = parse_tokens(shell.token);
	if (cmds_match(cmds, expected))
		printf("[PASS] %s\n", label);
	else
		printf("[FAIL] %s\n", label);
	printf("  input    : %s\n", input);
	print_cmds_result(cmds, expected);
	free_cmds(cmds);
	free_tokens(shell.token);
}

/**
 * @brief Teste les pipes transformes en plusieurs t_cmd chainees.
 */
static void	test_pipe_cases(void)
{
	char	*pipe_first[] = {"echo", "hello", NULL};
	char	*pipe_second[] = {"wc", "-c", NULL};
	char	*triple_first[] = {"echo", "hello", NULL};
	char	*triple_second[] = {"grep", "h", NULL};
	char	*triple_third[] = {"wc", "-l", NULL};
	char	*redir_first[] = {"cat", NULL};
	char	*redir_second[] = {"grep", "hello", NULL};
	char	**pipe[] = {pipe_first, pipe_second, NULL};
	char	**triple[] = {triple_first, triple_second, triple_third, NULL};
	char	**redir[] = {redir_first, redir_second, NULL};

	create_test_file("/tmp/minishell_parser_infile");
	test_pipe_case("pipe -> two linked commands",
		"echo hello | wc -c", pipe);
	test_pipe_case("pipes -> three linked commands",
		"echo hello | grep h | wc -l", triple);
	test_pipe_case("pipe + redirections skipped from args",
		"cat < /tmp/minishell_parser_infile | grep hello > /tmp/minishell_parser_out",
		redir);
}

/**
 * @brief Lance le pipeline parser et retourne la liste de commandes creee.
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
	return (parse_tokens(shell->token));
}

/**
 * @brief Verifie que les redirections ouvrent bien fd_in ou fd_out.
 */
static void	test_redir_fd_case(char *label, char *input, bool has_in,
	bool has_out)
{
	t_shell	shell;
	t_env	user;
	t_env	home;
	t_cmd	*cmds;

	cmds = parse_test_line(input, &shell, &user, &home);
	if (cmds && (cmds->fd_in != 0) == has_in
		&& (cmds->fd_out != 1) == has_out)
		printf("[PASS] %s\n", label);
	else
		printf("[FAIL] %s\n", label);
	printf("  input  : %s\n", input);
	if (cmds)
		printf("  fd_in  : %d\n  fd_out : %d\n", cmds->fd_in, cmds->fd_out);
	free_cmds(cmds);
	free_tokens(shell.token);
}

/**
 * @brief Verifie qu'une redirection input invalide fait echouer le parser.
 */
static void	test_redir_error_case(void)
{
	t_shell	shell;
	t_env	user;
	t_env	home;
	t_cmd	*cmds;

	cmds = parse_test_line("cat < /tmp/minishell_missing_input", &shell,
			&user, &home);
	if (!cmds)
		printf("[PASS] missing input redir -> parser error\n");
	else
		printf("[FAIL] missing input redir -> parser error\n");
	free_cmds(cmds);
	free_tokens(shell.token);
}

/**
 * @brief Teste les fd ouverts par les redirections du parser.
 */
static void	test_redir_fds(void)
{
	create_test_file("/tmp/minishell_parser_infile");
	test_redir_fd_case("input redir opens fd_in",
		"cat < /tmp/minishell_parser_infile", true, false);
	test_redir_fd_case("output redir opens fd_out",
		"echo hi > /tmp/minishell_parser_out", false, true);
	test_redir_fd_case("append redir opens fd_out",
		"echo hi >> /tmp/minishell_parser_log", false, true);
	test_redir_fd_case("last output redir wins",
		"echo hi > /tmp/minishell_parser_a > /tmp/minishell_parser_b",
		false, true);
	test_redir_error_case();
}

/**
 * @brief Teste plusieurs commandes simples transformees en cmd_and_args.
 */
static void	test_cmd_args(void)
{
	char	*plain[] = {"echo", "hello", NULL};
	char	*quotes[] = {"echo", "chloe", "$USER", NULL};
	char	*status[] = {"echo", "127", NULL};
	char	*missing[] = {"echo", "", "suffix", NULL};
	char	*digit[] = {"echo", "USER", "2USER", NULL};
	char	*redir_out[] = {"echo", "hi", NULL};
	char	*redir_in[] = {"cat", NULL};
	char	*append[] = {"echo", "hi", NULL};
	char	*heredoc[] = {"cat", NULL};

	*get_status() = 127;
	printf("\n=== PARSER CMD_AND_ARGS ===\n");
	create_test_file("/tmp/minishell_parser_infile");
	test_cmd_args_case("plain words -> echo hello",
		"echo hello", plain);
	test_cmd_args_case("quotes + expansion -> echo chloe $USER",
		"echo \"$USER\" '$USER'", quotes);
	test_cmd_args_case("status expansion -> echo 127",
		"echo $?", status);
	test_cmd_args_case("missing variable -> empty arg",
		"echo $MISSING suffix", missing);
	test_cmd_args_case("digit expansion -> $2USER / $12USER",
		"echo $2USER $12USER", digit);
	test_cmd_args_case("redir out skipped from args",
		"echo hi > /tmp/minishell_parser_out", redir_out);
	test_cmd_args_case("redir in skipped from args",
		"cat < /tmp/minishell_parser_infile", redir_in);
	test_cmd_args_case("append skipped from args",
		"echo hi >> /tmp/minishell_parser_log", append);
	test_cmd_args_case("heredoc delimiter skipped from args",
		"cat << EOF", heredoc);
	test_pipe_cases();
	test_redir_fds();
}

/**
 * @brief Version locale de get_status pour tester $? sans le vrai main.
 */
int	*get_status(void)
{
	static int	status;

	return (&status);
}

/**
 * @brief Lance les tests temporaires du parser.
 */
int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	(void)env;
	test_cmd_args();
	return (0);
}
