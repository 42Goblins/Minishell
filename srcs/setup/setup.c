/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 02:57:32 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/18 04:07:59 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	safe_add_var(t_env **env, char *key, char *value)
{
	if (!key || !value)
		return (free(key), free(value), 0);
	return (add_new_var(env, key, value));
}

int	update_shlvl(t_env *env)
{
	char	*shlvl_value;
	int		shlvl_int;

	shlvl_value = get_env_value(env, "SHLVL");
	if (!shlvl_value)
	{
		if (!update_env_vars(&env, "SHLVL", ft_strdup("1")))
			return (1);
		return (0);
	}
	shlvl_int = ft_atoi(shlvl_value) + 1;
	if (shlvl_int < 0)
		shlvl_int = 0;
	else if (shlvl_int > 1000)
	{
		ft_putstr_fd("minishell: warning: ", STDERR_FILENO);
		ft_putstr_fd("shell level too high, resetting to 1\n", STDERR_FILENO);
		shlvl_int = 1;
	}
	shlvl_value = ft_itoa(shlvl_int);
	if (!shlvl_value)
		return (1);
	set_env_value(env, "SHLVL", shlvl_value);
	free(shlvl_value);
	return (0);
}

char	*safe_getcwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (ft_strdup(""));
	return (cwd);
}

int	ensure_pwd(t_shell *shell)
{
	char	*pwd;

	pwd = get_env_value(shell->env, "PWD");
	if (pwd)
		return (0);
	pwd = safe_getcwd();
	if (!safe_add_var(&shell->env, ft_strdup("PWD"), pwd))
		return (free(pwd), 1);
	return (0);
}

int	create_minimal_env(t_shell *shell)
{
	if (!safe_add_var(&shell->env, ft_strdup("PWD"), safe_getcwd()))
		return (1);
	if (!safe_add_var(&shell->env, ft_strdup("SHLVL"), ft_strdup("0")))
		return (1);
	if (!safe_add_var(&shell->env, ft_strdup("PATH"), ft_strdup(DEFAULT_PATH)))
		return (1);
	return (0);
}

int	setup(t_shell *shell, char **env)
{
	shell->token = NULL;
	shell->cmds = NULL;
	shell->env = NULL;
	if (env == NULL || env[0] == NULL)
	 {
		if (create_minimal_env(shell) != 0)
			return (1);
	 }
	else if (setup_env(shell, env) != 0)
		return (1);
	if (ensure_pwd(shell) != 0)
		return (1);
	if (update_shlvl(shell->env) != 0)
		return (1);
	//if (setup_signals(shell) != 0)
	//	return (1);
	*get_status() = 0;
	return (0);
}






























































































