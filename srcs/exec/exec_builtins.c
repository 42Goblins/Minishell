/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 16:11:59 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/04 03:22:43 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_is_builtins(char *cmd)
{
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	return (0);
}

void	exec_builtins(t_shell *shell, t_cmd *cmd)
{
	if (ft_strncmp(cmd->cmd_and_args[0], "cd", 3) == 0)
		*get_status() = exec_cd(shell, cmd->cmd_and_args);
	else if (ft_strncmp(cmd->cmd_and_args[0], "echo", 5) == 0)
		*get_status() = exec_echo(cmd->cmd_and_args);
	else if (ft_strncmp(cmd->cmd_and_args[0], "env", 4) == 0)
		*get_status() = exec_env(shell->env, cmd->cmd_and_args);
	else if (ft_strncmp(cmd->cmd_and_args[0], "pwd", 4) == 0)
		*get_status() = exec_pwd();
	else if (ft_strncmp(cmd->cmd_and_args[0], "unset", 6) == 0)
		*get_status() = exec_unset(&shell->env, cmd->cmd_and_args);
	else if (ft_strncmp(cmd->cmd_and_args[0], "exit", 5) == 0)
		*get_status() = exec_exit(cmd->cmd_and_args);
	else if (ft_strncmp(cmd->cmd_and_args[0], "export", 7) == 0)
		*get_status() = exec_export(&shell->env, cmd->cmd_and_args);
}
