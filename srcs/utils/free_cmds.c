/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 16:29:06 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/01 16:33:11 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees a full command list and closes remaining redirection fds.
 *
 * The exec part may close duplicated fds in child processes, but the parent
 * still owns the fds stored in t_cmd until this cleanup runs.
 */
void	free_cmds(t_cmd *cmds)
{
	t_cmd	*tmp;

	while (cmds)
	{
		tmp = cmds->next;
		if (cmds->fd_in != 0)
			close(cmds->fd_in);
		if (cmds->fd_out != 1)
			close(cmds->fd_out);
		free_tab(cmds->cmd_and_args);
		free(cmds->path);
		free(cmds);
		cmds = tmp;
	}
}
