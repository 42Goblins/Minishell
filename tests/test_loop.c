/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 02:57:43 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/28 04:37:55 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Ce fichier sert a tester une fausse boucle minishell en local, sans toucher
 * au vrai main.c.
 *
 * Le but est de verifier ma pipeline avant exec :
 * readline -> tokenizer -> expand_tokens -> remove_quotes_from_tokens
 * -> validate_syntax -> parse_tokens -> affichage de cmd_and_args.
 *
 * Je fais ca ici pour eviter les conflits avec le main de Dounia, qui est
 * encore en train d'evoluer avec l'exec, les builtins, les fd, les signaux et
 * le status.
 *
 * Pour l'instant cette loop ne lance pas encore l'exec. Elle sert surtout a
 * voir si ma partie prepare bien les tokens et les commandes.
 */

void	print_cmd_args(char **cmd_and_args);
void	free_test_cmd(t_cmd *cmd);

/**
 * @brief Affiche les tokens produits par la pipeline locale.
 */
static void	print_tokens(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		printf("type: %d | value: %s | had_quotes: %d\n",
			current->type, current->value, current->had_quotes);
		current = current->next;
	}
}

/**
 * @brief Version locale de get_status pour tester $? sans le vrai main.
 */
int	*get_status(void)
{
	static int	status;

	return (&status);
}

/**
 * @brief Lance une mini boucle readline pour tester la pipeline avant exec.
 */
int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	t_shell	shell;
	char	*line;

	setup_env(&shell, env);
	shell.token = NULL;
	shell.cmds = NULL;
	shell.path = NULL;
	while (1)
	{
		line = readline("minishell-test> ");
		if (!line)
			break ;
		if (line[0] != '\0')
			add_history(line);
		shell.token = NULL;
		if (tokenizer(line, &shell) == 0)
		{
			if (expand_tokens(shell.token, shell.env) == 0)
			{
				if (remove_quotes_from_tokens(shell.token) == 0)
				{
					print_tokens(shell.token);
					if (validate_syntax(shell.token) == 0)
					{
						shell.cmds = parse_tokens(shell.token);
						if (shell.cmds)
							print_cmd_args(shell.cmds->cmd_and_args);
						else
							printf("parser error\n");
					}
					else
						printf("syntax error\n");
				}
				else
					printf("quote error\n");
			}
			else
				printf("expansion error\n");
		}
		else
			printf("tokenizer error\n");
		free_test_cmd(shell.cmds);
		shell.cmds = NULL;
		free_tokens(shell.token);
		shell.token = NULL;
		free (line);
	}
	return (0);
}

/**
 * @brief Affiche le tableau cmd_and_args cree par le parser.
 */
void	print_cmd_args(char **cmd_and_args)
{
	int i;

	i = 0;
	while (cmd_and_args[i])
	{
		printf("index : %d, string : %s\n", i, cmd_and_args[i]);
		i++;
	}
}

/**
 * @brief Libere une commande simple creee par la mini boucle de test.
 */
void	free_test_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	free_tab(cmd->cmd_and_args);
	free(cmd->path);
	free(cmd);
}
