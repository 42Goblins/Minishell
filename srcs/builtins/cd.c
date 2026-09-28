/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:34:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/27 23:52:04 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_env_pwd(t_env **env)
{
	char	*cwd;
	char	*oldpwd;
	char	*oldpwd_cpy;

	oldpwd_cpy = NULL;
	cwd = getcwd(NULL, 0);
	oldpwd = get_env_value(*env, "PWD");
	if (oldpwd)
		oldpwd_cpy = ft_strdup(oldpwd);
	if (cwd)
		update_env_vars(env, ft_strdup("PWD"), cwd);
	if (oldpwd_cpy)
		update_env_vars(env, ft_strdup("OLDPWD"), oldpwd_cpy);
}

int	go_to_oldpwd(t_env *env)
{
	char	*oldpwd;

	oldpwd = get_env_value(env, "OLDPWD");
	if (oldpwd)
	{
		if (chdir(oldpwd) == -1)
		{
			ft_putstr_fd("minishell: cd: ", 2);
			return (perror(oldpwd), 1);
		}
		ft_putendl_fd(oldpwd, STDOUT_FILENO);
		update_env_pwd(&env);
	}
	else
		return (ft_putstr_fd("minishell: cd: OLDPWD not set\n", 2), 1);
	return (0);
}

int	go_to_home_dir(t_env *env)
{
	char	*home;

	home = get_env_value(env, "HOME");
	if (home)
	{
		if (chdir(home) == -1)
		{
			ft_putstr_fd("minishell: cd: ", 2);
			return (perror(home), 1);
		}
		update_env_pwd(&env);
	}
	else
		return (ft_putstr_fd("cd: HOME not set\n", 2), 1);
	return (0);
}

/**
 * @brief Changes directory using a relative or absolute path.
 */
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
	update_env_pwd(&shell->env);
	return (0);
}
