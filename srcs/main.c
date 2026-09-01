/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:34:41 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/01 06:38:56 by dgeara           ###   ########.fr       */
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

int	*get_status(void)
{
	static int	status;

	return (&status);
}

void set_shell(t_shell *shell, t_cmd *cmd, char **env)
{
	//shell->sig = (t_sig){0};       		 // init des signaux à zéro/valeurs par défaut
	setup_env(shell, env);     // conversion char** -> t_env* (ta fonction de parsing d'env)
	shell->token = NULL;            // pas de tokens résiduels après exécution de cd
	//shell->env_for_exec = env;      // tableau d'env brut pour un futur execve (pas utilisé par cd, builtin)
	shell->cmds = cmd;           // pas de parser actif une fois la commande traitée
	shell->path = NULL;             // cd ne cherche pas de path binaire, c'est un builtin
}
/* void set_cmd(t_cmd *cmd)
{
	static char *cd_args[] = {"/bin/ls", NULL};
	cmd->cmd_and_args = cd_args;   	// ou {"cd", NULL} si c'est un char **
	cmd->path = NULL;               // cd est un builtin, pas de path binaire à chercher
	cmd->fd_in = 0;                 // STDIN_FILENO (pas de redirection)
	cmd->fd_out = 1;                // STDOUT_FILENO (pas de redirection)
	cmd->is_builtin = 0;            // true, cd est bien un builtin
	cmd->access_check = 1;          // true, on considère l'accès valide (rien à checker sur un builtin)
	cmd->next = NULL;               // pas de pipe, donc pas de commande suivante
}




int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	t_cmd cmd;
	t_shell shell;

	set_cmd(&cmd);
	set_shell(&shell, &cmd, env);

	// exec_single_external(shell.cmds, env);
	// exec_pipeline(shell.cmds, env);

	launch_exec(&shell, &cmd);
	
	 printf("oldpwd: %s\n", getcwd(NULL, 0));
	exec_builtins(&shell, shell.cmds);
	printf("newpwd: %s\n", getcwd(NULL, 0)); 
		
	//return (0);
//} */



static t_cmd	*make_cmd(char **args, int is_builtin, t_cmd *next)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->cmd_and_args = args;
	cmd->path = NULL;
	cmd->fd_in = 0;
	cmd->fd_out = 1;
	cmd->is_builtin = is_builtin;
	cmd->access_check = 1;
	cmd->next = next;
	return (cmd);
}

int	main(int ac, char **av, char **env)
{
	t_shell	shell;
	t_cmd	*pipeline;
	static char	*args_ls[] = {"ls", NULL};
	static char	*args_grep[] = {"grep", ".c", NULL};
	static char	*args_wc[] = {"wc", "-l", NULL};

	(void)ac;
	(void)av;
	setup_env(&shell, env);
	shell.token = NULL;
	shell.path = NULL;

	/* construit la pipeline en partant de la fin : ls | grep .c | wc -l */
	pipeline = make_cmd(args_wc, 0, NULL);
	pipeline = make_cmd(args_grep, 0, pipeline);
	pipeline = make_cmd(args_ls, 0, pipeline);

	shell.cmds = pipeline;
	launch_exec(&shell, pipeline);

	printf("exit status: %d\n", *get_status());
	return (0);
}
