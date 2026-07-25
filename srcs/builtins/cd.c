/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:34:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/07/25 03:29:55 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	set_env_value(t_env *env, const char *key, const char *value)
{
	while (env)
	{
		if (ft_strncmp(env->key, key, ft_strlen(key)) == 0)
		{
			free(env->value);
			env->value = ft_strdup(value);
			return ;
		}
		env = env->next;
	}
}

void	update_env(t_env *env)
{
	char	*cwd;
	char	*oldpwd;

	cwd = getcwd(NULL, 0);
	oldpwd = get_env_value(env, "PWD");
	if (cwd)
	{
		set_env_value(env, "PWD", cwd);
		free(cwd);
	}
	if (oldpwd)
		set_env_value(env, "OLDPWD", oldpwd);
}

char	*get_env_value(t_env *env, const char *key)
{
	while (env)
	{
		if (ft_strncmp(env->key, key, ft_strlen(key)) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

void	go_to_oldpwd(t_env *env)
{
	char	*oldpwd;

	oldpwd = get_env_value(env, "OLDPWD");
	if (oldpwd)
		chdir(oldpwd);
	else
		ft_putstr_fd("cd: OLDPWD not set\n", 2);
}

void	go_to_home_dir(t_env *env)
{
	char	*home;

	home = get_env_value(env, "HOME");
	if (home)
		chdir(home);
	else
		ft_putstr_fd("cd: HOME not set\n", 2);
}

/* @brief */
int	exec_cd(t_shell *shell, char **cmd)
{
	if (cmd[2])
		return (ft_putstr_fd("minishell: cd: too many arguments\n", 2), 0);
	if (!cmd[1] || (ft_strncmp(cmd[1], "~", 2) == 0))
		return (go_to_home_dir(shell->env), 0);
	if (ft_strncmp(cmd[1], "-", 2) == 0)
		return (go_to_oldpwd(shell->env), 0);
	if (chdir(cmd[1]) == -1)
	{
		ft_putstr_fd("minishell: cd:", 2);
		perror(cmd[1]);
	}
	update_env(shell->env);
	return (0);
}
