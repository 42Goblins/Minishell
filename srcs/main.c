/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:34:41 by dgeara            #+#    #+#             */
/*   Updated: 2026/08/07 03:06:06 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* int	main(int ac, char **av, char **env)
{
	if (ac != 1 || av[0] == NULL)
		return (1);
	if(env)
		setup_env(env);
	else
		create_env(env);
	
	return (0);
} */

void set_shell(t_shell *shell, t_cmd *cmd, char **env)
{
	//shell->sig = (t_sig){0};       		 // init des signaux à zéro/valeurs par défaut
	setup_env(shell, env);     // conversion char** -> t_env* (ta fonction de parsing d'env)
	shell->token = NULL;            // pas de tokens résiduels après exécution de cd
	//shell->env_for_exec = env;      // tableau d'env brut pour un futur execve (pas utilisé par cd, builtin)
	shell->cmds = cmd;           // pas de parser actif une fois la commande traitée
	shell->path = NULL;             // cd ne cherche pas de path binaire, c'est un builtin
}
void set_cmd(t_cmd *cmd)
{
	static char *cd_args[] = {"ls", NULL};
	cmd->cmd_and_args = cd_args;   	// ou {"cd", NULL} si c'est un char **
	cmd->path = NULL;               // cd est un builtin, pas de path binaire à chercher
	cmd->fd_in = 0;                 // STDIN_FILENO (pas de redirection)
	cmd->fd_out = 1;                // STDOUT_FILENO (pas de redirection)
	cmd->is_builtin = 1;            // true, cd est bien un builtin
	cmd->access_check = 1;          // true, on considère l'accès valide (rien à checker sur un builtin)
	cmd->next = NULL;               // pas de pipe, donc pas de commande suivante
}
void exec_builtins(t_shell *shell, t_cmd *cmd)
{
	if (ft_strncmp(cmd->cmd_and_args[0], "cd", 3) == 0)
		exec_cd(shell, cmd->cmd_and_args);
	else if (ft_strncmp(cmd->cmd_and_args[0], "echo", 5) == 0)
		exec_echo(cmd->cmd_and_args);
	else if (ft_strncmp(cmd->cmd_and_args[0], "env", 4) == 0)
		exec_env(shell->env, cmd->cmd_and_args);
	else if (ft_strncmp(cmd->cmd_and_args[0], "pwd", 4) == 0)
		exec_pwd();
	else if (ft_strncmp(cmd->cmd_and_args[0], "unset", 6) == 0)
		exec_unset(&shell->env, cmd->cmd_and_args);
	else if (ft_strncmp(cmd->cmd_and_args[0], "exit", 5) == 0)
		(exec_exit(cmd->cmd_and_args));
		else if (ft_strncmp(cmd->cmd_and_args[0], "export", 7) == 0)
		(exec_export(&shell->env, cmd->cmd_and_args));
	else
		ft_putstr_fd("minishell: command not found\n", 2);
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
	pid_t pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return ;
	}
	if (pid == 0)
		exec_external(cmd, env);
	/* waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		all->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		all->exit_status = 128 + WTERMSIG(status); */
}
int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	t_cmd cmd;
	t_shell shell;

	set_cmd(&cmd);
	set_shell(&shell, &cmd, env);

	exec_single_external(shell.cmds, env);
	
	/* printf("oldpwd: %s\n", getcwd(NULL, 0));
	exec_builtins(&shell, shell.cmds);
	printf("newpwd: %s\n", getcwd(NULL, 0)); */
		
	return (0);
}


