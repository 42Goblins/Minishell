/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 18:38:33 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/18 02:50:05 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cmd	*parse_tokens(t_token *tokens)
{
	if (!tokens)
		return (NULL);
	return (create_cmd_node(tokens));
}

int	count_cmd_args(t_token *tokens)
{
	t_token	*current;
	int		count;

	count = 0;
	current = tokens;
	while (current)
	{
		if (current->type == T_PIPE)
			return (count);
		if (current->type == T_WORD)
			count++;
		current = current->next;
	}
	return (count);
}

char	**create_cmd_args(t_token *tokens)
{
	t_token	*current;
	int		i;
	char	**cmd_and_args;

	cmd_and_args = malloc(sizeof(char *) * (count_cmd_args(tokens) + 1));
	if (!cmd_and_args)
		return (NULL);
	current = tokens;
	i = 0;
	while (current && current->type != T_PIPE)
	{
		if (current->type == T_WORD)
		{
			cmd_and_args[i] = ft_strdup(current->value);
			if (cmd_and_args[i] == NULL)
			{
				cmd_and_args[i] = NULL;
				return (free_tab(cmd_and_args), NULL);
			}
			i++;
		}
		current = current->next;
	}
	cmd_and_args[i] = NULL;
	return (cmd_and_args);
}

t_cmd	*create_cmd_node(t_token *tokens)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->cmd_and_args = NULL;
	cmd->path = NULL;
	cmd->fd_in = 0;
	cmd->fd_out = 1;
	cmd->is_builtin = false;
	cmd->access_check = false;
	cmd->next = NULL;
	cmd->cmd_and_args = create_cmd_args(tokens);
	if (!cmd->cmd_and_args)
		return (free(cmd), NULL);
	return (cmd);
}
