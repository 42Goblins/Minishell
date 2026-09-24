/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 20:22:24 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/26 14:48:04 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Counts the number of nodes in an t_env list.
 */
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

/**
 * @brief Converts the t_shell env list into an execve-compatible
 * "KEY=VALUE" array, terminated by NULL.
 */
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

int	handle_direct_path_error(char *cmd)
{
	struct stat	info;

	if (ft_strchr(cmd, '/') == NULL)
		return (0);
	if (stat(cmd, &info) == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(cmd);
		exit(127);
	}
	if (S_ISDIR(info.st_mode))
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": Is a directory\n", 2);
		exit(126);
	}
	if (access(cmd, X_OK) == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(cmd);
		exit(126);
	}
	return (0);
}

/**
 * @brief Find cmd's path and replaces the current process
 * with it via execve.
 *
 * Meant to run inside an already-forked child: it always exits the
 * process, whether the command is not found, not executable, or
 * successfully launched.
 */
void	exec_external(t_cmd *cmd, t_env *env)
{
	char	*path;
	char	**env_tab;
	

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	env_tab = NULL;
	handle_direct_path_error(cmd->cmd_and_args[0]);
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
}

/**
 * @brief Forks and runs an external command outside of any
 * pipeline, then waits for it and stores its exit status.
 */
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
	{
		set_fds(cmd, -1, NULL);
		exec_external(cmd, env);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		*get_status() = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		*get_status() = 128 + WTERMSIG(status);
<<<<<<< HEAD
		print_signal_message(WTERMSIG(status));
	}
=======
	/* if (WTERMSIG(status) == SIGINT)
			printf("\n");
		else if (WTERMSIG(status) == SIGQUIT)
			ft_putstr_fd("Quit (core dumped)\n", STDERR_FILENO); */
>>>>>>> 4f2f282 (chore: de la norm et suite correction premier audit)
}
