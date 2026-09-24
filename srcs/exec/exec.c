/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 02:40:17 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/23 23:44:12 by cmauley          ###   ########.fr       */
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

/**
 * @brief Dispatches a parsed command list to the right execution path:
 * a lone builtin, a lone external command, or a full pipeline.
 */
void	launch_exec(t_shell *shell, t_cmd *cmds)
{
	int	cmd_count;

	if (!cmds || !cmds->cmd_and_args || !cmds->cmd_and_args[0])
		return ;
	ignore_exec_signals();
	cmd_count = count_cmds(cmds);
	if (cmd_count == 1 && cmds->is_builtin)
		exec_single_builtins(shell, cmds);
	else if (cmd_count == 1)
		exec_single_external(cmds, shell->env);
	else
		exec_pipeline(shell, cmds);
	setup_signals();
}
