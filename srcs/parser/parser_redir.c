/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 01:52:56 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/03 01:53:31 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Opens redirection files for one command and stores their fds.
 *
 * The scan stops at the next pipe. If several redirections target the same
 * side, the last one replaces the previous fd.
 */
int	open_redirections(t_cmd *cmd, t_token *tokens)
{
	t_token	*current;
	int		fd;

	current = tokens;
	if (!current)
		return (0);
	while (current && current->type != T_PIPE)
	{
		if (current->type == T_REDIR_IN)
		{
			fd = open(current->next->value, O_RDONLY);
			if (fd == -1)
				return (1);
			if (cmd->fd_in != 0)
				close(cmd->fd_in);
			cmd->fd_in = fd;
			current = current->next->next;
		}
		else if (current->type == T_REDIR_OUT)
		{
			fd = open(current->next->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
				return (1);
			if (cmd->fd_out != 1)
				close(cmd->fd_out);
			cmd->fd_out = fd;
			current = current->next->next;
		}
		else if (current->type == T_APPEND)
		{
			fd = open(current->next->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
				return (1);
			if (cmd->fd_out != 1)
				close(cmd->fd_out);
			cmd->fd_out = fd;
			current = current->next->next;
		}
		else
			current = current->next;
	}
	return (0);
}
