/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:34:41 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/16 02:41:47 by dgeara           ###   ########.fr       */
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
	expand_tokens(shell->token, shell->env); //check se que ça return ?
	remove_quotes_from_tokens(shell->token);
	if (validate_syntax(shell->token))
	{
		*get_status() = 2;
		ft_putstr_fd("minishell: syntax error\n", STDERR_FILENO);
		return ;
	}
	shell->cmds = parse_tokens(shell->token);
	launch_exec(shell, shell->cmds);
}

int	launch_loop(t_shell *shell)
{
	char	*line;

	line = readline("minishell$ ");
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
	setup_env(&shell, env);
	shell.token = NULL;
	shell.cmds = NULL;
	// setup_signals(&shell);
	while (launch_loop(&shell) == 0)
		;
	clean_exit(&shell, *get_status());
	return (0);
}
