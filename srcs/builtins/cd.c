/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:34:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/21 01:55:18 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_env_pwd(t_env *env)
{
	char	*cwd;
	char	*oldpwd;
	char	*oldpwd_cpy;

	oldpwd_cpy = NULL;
	cwd = getcwd(NULL, 0);
	oldpwd = get_env_value(env, "PWD");
	if (oldpwd)
		oldpwd_cpy = ft_strdup(oldpwd);
	if (cwd)
		set_env_value(env, "PWD", cwd);
	if (oldpwd_cpy)
		set_env_value(env, "OLDPWD", oldpwd_cpy);
	free(cwd);
}

int	go_to_oldpwd(t_env *env)
{
	char	*oldpwd;

	oldpwd = get_env_value(env, "OLDPWD");
	if (oldpwd)
	{
		chdir(oldpwd);
		update_env_pwd(env);
	}
	else
		return (ft_putstr_fd("cd: OLDPWD not set\n", 2), 1);
	return (0);
}

int	go_to_home_dir(t_env *env)
{
	char	*home;

	home = get_env_value(env, "HOME");
	if (home)
	{
		chdir(home);
		update_env_pwd(env);
	}
	else
		return (ft_putstr_fd("cd: HOME not set\n", 2), 1);
	return (0);
}

/* @brief */
int	exec_cd(t_shell *shell, char **cmd)
{
	if (cmd[1] && cmd[2])
		return (ft_putstr_fd("minishell: cd: too many arguments\n", 2), 1);
	if (!cmd[1] || (ft_strncmp(cmd[1], "~", 2) == 0))
		return (go_to_home_dir(shell->env));
	if (ft_strncmp(cmd[1], "-", 2) == 0)
		return (go_to_oldpwd(shell->env));
	if (chdir(cmd[1]) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		return (perror(cmd[1]), 1);
	}
	update_env_pwd(shell->env);
	return (0);
}
