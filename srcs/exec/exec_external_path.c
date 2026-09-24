/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_path.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 03:57:59 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/18 05:51:12 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks whether cmd exists and is executable inside dir,
 * returning its full path if so and null if it doesn't exist or
 * doesn't have acces.
 */
char	*try_path(char *dir, char *cmd)
{
	struct stat	info;
	char		*tmp;
	char		*full;

	tmp = ft_strjoin(dir, "/");
	full = ft_strjoin(tmp, cmd);
	free(tmp);
	if (stat(full, &info) == 0 && !S_ISDIR(info.st_mode)
		&& access(full, X_OK) == 0)
		return (full);
	free(full);
	return (NULL);
}

/**
 * @brief Retrieves the value of the PATH variable from the env list.
 */
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

/**
 * @brief Looks up a command name in $PATH and returns its full path.
 *
 * Absolute or relative paths (starting with / or ./) are checked
 * directly instead. The returned string is always allocated.
 */
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
