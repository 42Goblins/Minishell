/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 02:57:43 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/28 04:37:55 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_cmd_args(char **cmd_and_args);
void	free_test_cmd(t_cmd *cmd);

static void	print_tokens(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		printf("type: %d | value: %s | had_quotes: %d\n",
			current->type, current->value, current->had_quotes);
		current = current->next;
	}
}

int	*get_status(void)
{
	static int	status;

	return (&status);
}

int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	t_shell	shell;
	char	*line;

	setup_env(&shell, env);
	shell.token = NULL;
	shell.cmds = NULL;
	shell.path = NULL;
	while (1)
	{
		line = readline("minishell-test> ");
		if (!line)
			break ;
		if (line[0] != '\0')
			add_history(line);
		shell.token = NULL;
		if (tokenizer(line, &shell) == 0)
		{
			if (expand_tokens(shell.token, shell.env) == 0)
			{
				if (remove_quotes_from_tokens(shell.token) == 0)
				{
					print_tokens(shell.token);
					shell.cmds = parse_tokens(shell.token);
					if (shell.cmds)
						print_cmd_args(shell.cmds->cmd_and_args);
					else
						printf("parser error\n");
				}
				else
					printf("quote error\n");
			}
			else
				printf("expansion error\n");
		}
		else
			printf("tokenizer error\n");
		free_test_cmd(shell.cmds);
		shell.cmds = NULL;
		free_tokens(shell.token);
		shell.token = NULL;
		free (line);
	}
	return (0);
}

void	print_cmd_args(char **cmd_and_args)
{
	int i;

	i = 0;
	while (cmd_and_args[i])
	{
		printf("index : %d, string : %s\n", i, cmd_and_args[i]);
		i++;
	}
}

void	free_test_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	free_tab(cmd->cmd_and_args);
	free(cmd->path);
	free(cmd);
}
