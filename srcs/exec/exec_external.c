/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 20:22:24 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/08 03:57:56 by dgeara           ###   ########.fr       */
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

void	exec_external(t_cmd *cmd, t_env *env)
{
	char	*path;
	char	**env_tab;

	env_tab = NULL;
	path = find_path(cmd->cmd_and_args[0], env);
	if (!path) // NOPE problème si psq doit continuer la pipeline enft :(((
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
	exit(126); // return (getstatus = 126) ??
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
	if (WIFEXITED(status))
		*get_status() = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		*get_status() = 128 + WTERMSIG(status);
	// exit(126) ??
	// check mieux les exit pour external, si pas trouvé 127, si pas exécutable 
	// 126, sinon le status du fils ??
}
