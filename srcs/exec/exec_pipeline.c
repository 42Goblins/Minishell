/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 23:47:02 by dgeara            #+#    #+#             */
/*   Updated: 2026/08/19 02:19:12 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exec_pipeline(t_shell *shell, t_cmd *cmds)
{
	(void)shell;
	(void)cmds;
	int		pipe_fds[2];
	int		prev_fd;
	pid_t	*pids;
	int		i;

	prev_fd = -1;   // pas de fd d'entrée au départ (stdin hérité)
	pids = malloc(sizeof(pid_t) * count_cmds(cmds));
	i = 0;
	while (cmds)
	{
		if (cmds->next)
			run_pipe(shell, cmds, &prev_fd, &pids[i]);       // équivalent run_pipe
		else
			pids[i] = run_last_cmd(shell, cmds, prev_fd, &pids[i]);    // équivalent run_last_pipe
		cmds = cmds->next;
		i++;
	}
	wait_all_pids(pids, i);
	free(pids);
	
	exec_builtins(shell, cmds);
	
	if (pipe(pipe_fds) == -1)
		return (perror("pipe"), 1);
	pid = fork();
	if (pid < 0)
		return (perror("fork"), 1);
	if (pid == 0)
		child_process1(av, pipe_fds, envp);
	pid = fork();
	if (pid < 0)
		return (perror("fork"), 1);
	if (pid == 0)
		child_process2(av, pipe_fds, envp);
	close(pipe_fds[0]);
	close(pipe_fds[1]);
	wait(NULL);
	wait(NULL);
	return (0);
}