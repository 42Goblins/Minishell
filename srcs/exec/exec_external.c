/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 20:22:24 by dgeara            #+#    #+#             */
/*   Updated: 2026/08/28 03:17:26 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	env_len(t_env *env)
{
	int	i;

	i = 0;
	while (env)
	{
		i++;
		env = env->next;
	}
	return (i);
}

char	**t_env_to_tab(t_env *env)
{
	int		i;
	char	*tmp_env_tab;
	char	**env_tab;

	env_tab = malloc(sizeof(char *) * (env_len(env) + 1));
	if (!env_tab)
		return (NULL);
	i = 0;
	while (env)
	{
		tmp_env_tab = ft_strjoin(env->key, "=");
		env_tab[i] = ft_strjoin(tmp_env_tab, env->value);
		free(tmp_env_tab);
		env = env->next;
		i++;
	}
	env_tab[i] = NULL;
	return (env_tab);
}

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

char	*get_path(t_env *env)
{
	while (env)
	{
		if (ft_strcmp(env->key, "PATH") == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

char	*find_path(char *cmd, t_env *env)
{
	char	**dirs;
	char	*path;
	char	*result;
	int		i;

	if (cmd[0] == '/' || (cmd[0] == '.' && cmd[1] == '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
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

void	exec_external(t_cmd *cmd, t_env *env)
{
	char	*path;
	char	**env_tab;

	env_tab = NULL;
	path = find_path(cmd->cmd_and_args[0], env);
	if (!path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->cmd_and_args[0], STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		exit(127);
	}
	env_tab = t_env_to_tab(env);
	execve(path, cmd->cmd_and_args, env_tab);
	perror("execve");
	free(path);
	free_tab(env_tab);
	exit(126);
	// restore_original_signals

	//	if (WIFEXITED(status))
	//	*get_status() = WEXITSTATUS(status);
	//else if (WIFSIGNALED(status))
	//	*get_status() = 128 + WTERMSIG(status);

}

void	exec_single_external(t_cmd *cmd, t_env *env)
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
	// exit(126) ??
	// check mieux les exit pour external, si pas trouvé 127, si pas exécutable 
	// 126, sinon le status du fils ??
}
