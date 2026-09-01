/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 23:47:02 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/01 06:31:05 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void safe_close_fd(int fd)
{
	if (fd != -1)
		close(fd);
}

void wait_all_pids(pid_t last_pid)
{
	int status;
	pid_t pid;

	while ((pid = wait(&status)) > 0)
	{
		if (pid == last_pid)
		{
			if (WIFEXITED(status))
				*get_status() = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				*get_status() = 128 + WTERMSIG(status);
		}
	} // gerer le cas ou wait retourne -1 et errno == ECHILD ? et les signaux par ici ??
}

void	set_fds(t_cmd *cmds, int prev_fd, int pipefd[2])
{
	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (cmds->next)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
	}
	if (cmds->fd_in != 0)
	{
		dup2(cmds->fd_in, STDIN_FILENO);
		close(cmds->fd_in);
	}
	if (cmds->fd_out != 1)
	{
		dup2(cmds->fd_out, STDOUT_FILENO);
		close(cmds->fd_out);
	}
}

void	exec_cmd(t_shell *shell, t_cmd *cmds)
{
	// int status;

	if (cmds->is_builtin)
		exec_builtins(shell, cmds);
	else
		exec_external(cmds, shell->env);
	exit(*get_status());
}

int	exec_pipeline(t_shell *shell, t_cmd *cmds)
{
	pid_t	pid;
	int		pipefd[2];
	int		prev_fd;
 
	prev_fd = -1;
	while (cmds)
	{
		if (cmds->next && pipe(pipefd) == -1)
			return (perror("pipe"), safe_close_fd(prev_fd), 1);
		pid = fork();
		if (pid == -1)
			return (perror("fork"), safe_close_fd(prev_fd), 1); // close otherfds ?
		if (pid == 0)
		{
			set_fds(cmds, prev_fd, pipefd);
			exec_cmd(shell, cmds);
		}	
		else
		{ // close les fd inutiles dans le parent
			if (prev_fd != -1)
				close(prev_fd);
			if (cmds->next)
			{
				close(pipefd[1]);
				prev_fd = pipefd[0];
			}	
		}
		cmds = cmds->next;
	}
	safe_close_fd(prev_fd);
	wait_all_pids(pid);
	return (*get_status() ); // ou rien ?
}
