/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 20:22:24 by dgeara            #+#    #+#             */
/*   Updated: 2026/08/09 22:32:42 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_tab(char **tab)
{
	int	i;

	i = 0;
	while (tab[i])
		free(tab[i++]);
	free(tab);
}

char	*try_path(char *dir, char *cmd)
{
	char	*tmp;
	char	*full;

	tmp = ft_strjoin(dir, "/");
	full = ft_strjoin(tmp, cmd);
	free(tmp);
	if (access(full, X_OK) == 0)
		return (full);
	free(full);
	return (NULL);
}

char	*get_path(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
			return (env[i] + 5);
		i++;
	}
	return (NULL);
}

char	*find_path(char *cmd, char **env)
{
	char	**dirs;
	char	*path;
	char	*result;
	int		i;

	if (cmd[0] == '/' || (cmd[0] == '.' && cmd[1] == '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (cmd);
		return (NULL);
	}
	path = get_path(env);
	if (!path)
		return (NULL);
	dirs = ft_split(path, ':');
	i = 0;
	while (dirs[i])
	{
		result = try_path(dirs[i], cmd);
		if (result)
			return (free_tab(dirs), result);
		i++;
	}
	i = 0;
	return (free_tab(dirs), NULL);
}

void exec_external(t_cmd *cmd, char **env)
{
	char    *path;

	path = find_path(cmd->cmd_and_args[0], env);
	if (!path)
	{
		ft_putstr_fd(cmd->cmd_and_args[0], STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		exit(127);
	}
	execve(path, cmd->cmd_and_args, env);
	perror("execve");
	free(path);
	exit(126);
}

void exec_single_external(t_cmd *cmd, char **env)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return ;
	}
	if (pid == 0)
		exec_external(cmd, env);
	waitpid(pid, &status, 0);
	printf("%d\n", status);
	if (WIFEXITED(status))
		 *get_status() = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		*get_status() = 128 + WTERMSIG(status);
	printf("%d\n", *get_status());
}
