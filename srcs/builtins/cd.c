/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:34:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/07/20 03:54:22 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void exec_cd(t_shell *shell, t_cmd *cmd)
{
	//if rien après cd
	
	// move to la directory qui suit cd, + check que chdir pas flop !
	//et retourner le bon msg
		//if(!chdir(cmd->cmd_and_args[1]);
		//printf("cd: %s: %s\n", cmd->cmd_and_args[1], strerror(errno)); ???
		//perror ?? nah errno like real sheel

	//update env
	if (!cmd->cmd_and_args[1])
	{
		// No argument provided, change to home directory
		char *home = get_env_value(shell->env, "HOME");
		if (home)
			chdir(home);
		else
			printf("cd: HOME not set\n");
	}
	else
	{
		// Change to the specified directory
		if (chdir(cmd->cmd_and_args[1]) != 0)
			printf("cd: %s: %s\n", cmd->cmd_and_args[1], strerror(errno));
	}
}