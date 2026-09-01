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
 * @brief Frees a full command list and its allocated fields.
 */
void	free_cmds(t_cmd *cmds)
{
	t_cmd	*tmp;

	while (cmds)
	{
		tmp = cmds->next;
		free_tab(cmds->cmd_and_args);
		free(cmds->path);
		/* TODO: close fd_in/fd_out ici quand les redir seront implementees. */
		free(cmds);
		cmds = tmp;
	}
}
