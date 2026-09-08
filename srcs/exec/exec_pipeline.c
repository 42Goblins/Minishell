/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 23:47:02 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/08 03:53:26 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* void	safe_close_fd(int fd)
{
	if (fd != -1)
		close(fd);
} */

void	wait_all_pids(pid_t last_pid)
{
	int		status;
	pid_t	pid;

	pid = wait(&status);
	while (pid > 0) // pas legale avec la norminette
	{
		if (pid == last_pid)
		{
			if (WIFEXITED(status))
				*get_status() = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				*get_status() = 128 + WTERMSIG(status);
		}
		// gerer cas ou wait ret -1 et errno == ECHILD ? et les signx par ici ??
		//if (WIFSIGNALED(status) && WTERMSIG(status) == SIGQUIT)
		//	ft_putstr_fd("Quit (core dumped)\n", STDERR_FILENO);
		pid = wait(&status);
	}
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

pid_t	spawn_cmd(t_shell *shell, t_cmd *cmds, int *prev_fd, int pipefd[2])
{
	pid_t	pid;

	pipefd[0] = -1;
	pipefd[1] = -1;
	if (cmds->next && pipe(pipefd) == -1)
		return (perror("minishell: pipe"), -1);
	pid = fork();
	if (pid == -1)
		return (perror("minishell: fork"), -1);
	if (pid == 0)
	{
		set_fds(cmds, *prev_fd, pipefd);
		exec_cmd(shell, cmds);
	}
	else
	{
		safe_close_fd(prev_fd);
		if (cmds->next)
		{
			safe_close_fd(&pipefd[1]);
			*prev_fd = pipefd[0];
			pipefd[0] = -1;
		}
	}
	return (pid);
}

int	exec_pipeline(t_shell *shell, t_cmd *cmds)
{
	pid_t	pid;
	pid_t	last_pid;
	int		pipefd[2];
	int		prev_fd;

	prev_fd = -1;
	last_pid = -1;
	pipefd[0] = -1;
	pipefd[1] = -1;
	while (cmds)
	{
		pid = spawn_cmd(shell, cmds, &prev_fd, pipefd);
		if (pid == -1)
			break ;
		last_pid = pid;
		cmds = cmds->next;
	}
	safe_close_all_fd(&prev_fd, pipefd);
	wait_all_pids(last_pid);
	return (*get_status()); // ou rien ?
}
