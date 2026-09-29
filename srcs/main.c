/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:34:41 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/29 02:36:09 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees the current command and token data before the next prompt.
 */
void	reset_shell_state(t_shell *shell)
{
	free_cmds(shell->cmds);
	free_tokens(shell->token);
	shell->cmds = NULL;
	shell->token = NULL;
}

/**
 * @brief Tokenizes, expands, parses, and executes a command line.
 */
void	process_line(t_shell *shell, char *line)
{
	if (tokenizer(line, shell))
		return ;
	expand_tokens(shell->token, shell->env);
	remove_quotes_from_tokens(shell->token);
	if (validate_syntax(shell->token))
		return ;
	shell->cmds = parse_tokens(shell, shell->token, shell->env);
	launch_exec(shell, shell->cmds);
}

/**
 * @brief Reads and processes one command line from the interactive prompt.
 */
int	launch_loop(t_shell *shell)
{
	char	*line;

	line = read_input("minishell$ ");
	if (g_signal == SIGINT)
	{
		*get_status() = 130;
		g_signal = 0;
	}
	if (!line)
	{
		if (isatty(STDIN_FILENO))
			ft_putstr_fd("exit\n", STDOUT_FILENO);
		clean_exit(shell, *get_status());
	}
	shell->current_line = line;
	if (line[0] != '\0')
	{
		if (isatty(STDIN_FILENO))
			add_history(line);
		process_line(shell, line);
		reset_shell_state(shell);
	}
	shell->current_line = NULL;
	free(line);
	return (0);
}

/**
 * @brief Initializes the shell and starts the main interactive loop.
 */
int	main(int ac, char **av, char **env)
{
	t_shell	shell;

	printf("MAIN PID = %d\n", getpid());

	if (ac != 1 || av[0] == NULL)
		return (1);
	if (setup(&shell, env) != 0)
	{
		ft_putstr_fd("minishell: setup failed\n", STDERR_FILENO);
		clean_exit(&shell, 1);
	}
	while (launch_loop(&shell) == 0)
		;
	clean_exit(&shell, *get_status());
	return (0);
}
