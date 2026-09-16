/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 16:11:59 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/16 02:18:07 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks whether the command name matches one of the shell's
 * builtins (cd, echo, env, pwd, unset, exit, export).
 */
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

/**
 * @brief Runs the builtin matching cmd's name and stores its exit status.
 */
void	exec_builtins(t_shell *shell, t_cmd *cmd)
{
	if (ft_strcmp(cmd->cmd_and_args[0], "cd") == 0)
		*get_status() = exec_cd(shell, cmd->cmd_and_args);
	else if (ft_strcmp(cmd->cmd_and_args[0], "echo") == 0)
		*get_status() = exec_echo(cmd->cmd_and_args);
	else if (ft_strcmp(cmd->cmd_and_args[0], "env") == 0)
		*get_status() = exec_env(shell->env, cmd->cmd_and_args);
	else if (ft_strcmp(cmd->cmd_and_args[0], "pwd") == 0)
		*get_status() = exec_pwd();
	else if (ft_strcmp(cmd->cmd_and_args[0], "unset") == 0)
		*get_status() = exec_unset(&shell->env, cmd->cmd_and_args);
	else if (ft_strcmp(cmd->cmd_and_args[0], "exit") == 0)
		*get_status() = exec_exit(shell, cmd->cmd_and_args);
	else if (ft_strcmp(cmd->cmd_and_args[0], "export") == 0)
		*get_status() = exec_export(&shell->env, cmd->cmd_and_args);
}

void	exec_single_builtins(t_shell *shell, t_cmd *cmd)
{
	int	saved_stdin;
	int	saved_stdout;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	set_fds(cmd, -1, NULL);
	exec_builtins(shell, cmd);
	dup2(saved_stdin, STDIN_FILENO);
	safe_close_fd(&saved_stdin);
	dup2(saved_stdout, STDOUT_FILENO);
	safe_close_fd(&saved_stdout);
}
