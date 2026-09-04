/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:34:41 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/04 01:45:29 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	*get_status(void)
{
	static int	status;

	return (&status);
}

void	free_lst_env(t_env *env)
{
	t_env	*next;

	while (env)
	{
		next = env->next;
		free_t_env(env);
		env = next;
	}
}

void	free_lst_cmds(t_cmd *cmds)
{
	t_cmd	*next;

	while (cmds)
	{
		next = cmds->next;
		free_tab(cmds->cmd_and_args);
		free(cmds->path);
		free(cmds);
		cmds = next;
	}
}

void	reset_shell_state(t_shell *shell)
{
	free_lst_cmds(shell->cmds);
	free_tokens(shell->token);
	shell->cmds = NULL;
	shell->token = NULL;
}

void	process_line(t_shell *shell, char *line)
{
	tokenizer(line, shell);
	expand_tokens(shell->token, shell->env);
	remove_quotes_from_tokens(shell->token);
	validate_syntax(shell->token);
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
		return (1); // clean_exit(shell, *get_status());
	}
	if (line[0] != '\0')
	{
		add_history(line);
		process_line(shell, line);
		reset_shell_state(shell);
	}
	free(line);
	return (0); //or return get_status ??
}

int	main(int ac, char **av, char **env)
{
	t_shell	shell;

	if (ac != 1 || av[0] == NULL)
		return (1);
	// setup
	setup_env(&shell, env);
	shell.token = NULL;
	shell.cmds = NULL;
	// setup_signals(&shell);
	while (launch_loop(&shell) == 0)
		;
	//clean_exit (free_env, free_cmds, free_tokens, etc)
	free_lst_env(shell.env);
	clear_history();
	return (*get_status());
}

