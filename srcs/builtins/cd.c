/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:34:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/07/20 21:09:26 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void exec_cd(t_shell *shell, t_cmd *cmd)
{
	//if rien après cd
	if (!cmd->cmd_and_args[1])
	{
		char *home = get_env_value(shell->env, "HOME");
		if (home)
			chdir(home);
		else
			printf("cd: HOME not set\n");
	}
	
	if (cmd->cmd_and_args[2])
	{
		ft_putendl_fd("minishell: cd: too many arguments", 2);
		return (2);
	}
	// Change to the specified directory avec getcwd ?
	// move to la directory qui suit cd, + check que chdir pas flop !
	//et retourner le bon msg
		//if(!chdir(cmd->cmd_and_args[1]);
		//printf("cd: %s: %s\n", cmd->cmd_and_args[1], strerror(errno)); ???
		//perror ?? nah errno like real sheel
	if (!chdir(cmd->cmd_and_args[1]))
	{
		ft_putstr_fd("minishell: cd:", 2);
		perror(cmd->cmd_and_args[1]);
	}
	//update env
}