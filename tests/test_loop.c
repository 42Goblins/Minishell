/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 02:57:43 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/18 03:27:32 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
				print_tokens(shell.token);
			else
				printf("expansion error\n");
		}
		else
			printf("tokenizer error\n");
		free_tokens(shell.token);
		shell.token = NULL;
		free (line);
	}
	return (0);
}
