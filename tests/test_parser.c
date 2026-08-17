/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_parser.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 20:08:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/17 20:08:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Cree un petit environnement fake pour tester le parser.
 */
static void	init_test_env(t_env *user, t_env *home)
{
	home->key = "HOME";
	home->value = "/home/chloe";
	home->next = NULL;
	user->key = "USER";
	user->value = "chloe";
	user->next = home;
}

/**
 * @brief Compare le tableau cmd_and_args avec les strings attendues.
 */
static bool	args_match(char **args, char **expected)
{
	int	i;

	i = 0;
	while (args && expected && args[i] && expected[i])
	{
		if (ft_strcmp(args[i], expected[i]) != 0)
			return (false);
		i++;
	}
	return (args && expected && args[i] == NULL && expected[i] == NULL);
}

/**
 * @brief Affiche un tableau de strings sous forme ["a", "b", NULL].
 */
static void	print_args(char **args)
{
	int	i;

	i = 0;
	printf("[");
	while (args && args[i])
	{
		if (i > 0)
			printf(", ");
		printf("\"%s\"", args[i]);
		i++;
	}
	if (i > 0)
		printf(", ");
	printf("NULL]");
}

/**
 * @brief Libere une commande simple creee par le parser.
 */
static void	free_test_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	free_tab(cmd->cmd_and_args);
	free(cmd->path);
	free(cmd);
}

/**
 * @brief Lance le pipeline parser et compare cmd_and_args au resultat attendu.
 */
static void	test_cmd_args_case(char *label, char *input, char **expected)
{
	t_shell	shell;
	t_env	user;
	t_env	home;
	t_cmd	*cmd;

	init_test_env(&user, &home);
	shell.token = NULL;
	shell.env = &user;
	shell.cmds = NULL;
	if (tokenizer(input, &shell))
		return ((void)printf("[FAIL] tokenizer: %s\n", label));
	if (expand_tokens(shell.token, shell.env))
		return (free_tokens(shell.token), (void)printf("[FAIL] expansion: %s\n",
				label));
	if (remove_quotes_from_tokens(shell.token))
		return (free_tokens(shell.token), (void)printf("[FAIL] quotes: %s\n",
				label));
	cmd = parse_tokens(shell.token);
	if (!cmd)
		return (free_tokens(shell.token), (void)printf("[FAIL] parser: %s\n",
				label));
	if (args_match(cmd->cmd_and_args, expected))
		printf("[PASS] %s\n", label);
	else
		printf("[FAIL] %s\n", label);
	printf("  input    : %s\n", input);
	printf("  result   : ");
	print_args(cmd->cmd_and_args);
	printf("\n  expected : ");
	print_args(expected);
	printf("\n");
	free_test_cmd(cmd);
	free_tokens(shell.token);
}

/**
 * @brief Teste plusieurs commandes simples transformees en cmd_and_args.
 */
static void	test_cmd_args(void)
{
	char	*plain[] = {"echo", "hello", NULL};
	char	*quotes[] = {"echo", "chloe", "$USER", NULL};
	char	*status[] = {"echo", "127", NULL};
	char	*missing[] = {"echo", "", "suffix", NULL};
	char	*digit[] = {"echo", "USER", "2USER", NULL};

	*get_status() = 127;
	printf("\n=== PARSER CMD_AND_ARGS ===\n");
	test_cmd_args_case("plain words -> echo hello",
		"echo hello", plain);
	test_cmd_args_case("quotes + expansion -> echo chloe $USER",
		"echo \"$USER\" '$USER'", quotes);
	test_cmd_args_case("status expansion -> echo 127",
		"echo $?", status);
	test_cmd_args_case("missing variable -> empty arg",
		"echo $MISSING suffix", missing);
	test_cmd_args_case("digit expansion -> $2USER / $12USER",
		"echo $2USER $12USER", digit);
}

/**
 * @brief Lance les tests temporaires du parser.
 */
int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	(void)env;
	test_cmd_args();
	return (0);
}
