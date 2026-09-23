/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:34:41 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/23 23:09:57 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	reset_shell_state(t_shell *shell)
{
	free_cmds(shell->cmds);
	free_tokens(shell->token);
	shell->cmds = NULL;
	shell->token = NULL;
}

void	process_line(t_shell *shell, char *line)
{
	tokenizer(line, shell);
	expand_tokens(shell->token, shell->env);
	remove_quotes_from_tokens(shell->token);
	if (validate_syntax(shell->token))
		return ;
	shell->cmds = parse_tokens(shell->token, shell->env);
	launch_exec(shell, shell->cmds);
}

int	launch_loop(t_shell *shell)
{
	char	*line;

	line = readline("minishell$ ");
	if (g_signal == SIGINT)
	{
		*get_status() = 130;
		g_signal = 0;
	}
	if (!line)
	{
		ft_putstr_fd("exit\n", STDOUT_FILENO);
		clean_exit(shell, *get_status());
	}
	if (line[0] != '\0')
	{
		add_history(line);
		process_line(shell, line);
		reset_shell_state(shell);
	}
	free(line);
	return (0);
}

int	main(int ac, char **av, char **env)
{
	t_shell	shell;

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
