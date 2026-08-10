/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 02:40:17 by dgeara            #+#    #+#             */
/*   Updated: 2026/08/10 15:24:54 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_cmds(t_cmd *cmds)
{
	int	count;

	count = 0;
	while (cmds)
	{
		count++;
		cmds = cmds->next;
	}
	return (count);
}
void	launch_exec(t_shell shell, t_cmd cmds)
{
	int	cmd_count;
	
	cmd_count = count_cmds(&cmds);
	if (cmd_count == 1 && cmds.is_builtin)
		exec_builtins(&shell, &cmds);
	else if (cmd_count == 1)
		exec_external(&shell, &cmds);
	else
		exec_pipeline(&shell, &cmds);
}
