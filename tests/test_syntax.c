/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 03:20:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/01 03:20:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Ce fichier teste la validation syntaxique avant parser/exec.
 *
 * Il verifie que les lignes valides passent, et que les erreurs simples sont
 * refusees avant parse_tokens : pipe au debut/fin, double pipe, redirection
 * sans word apres, ou redirection suivie d'un autre operateur.
 */

/**
 * @brief Cree une shell minimale pour tester tokenizer + syntax.
 */
static void	init_test_shell(t_shell *shell)
{
	shell->env = NULL;
	shell->token = NULL;
	shell->cmds = NULL;
	shell->path = NULL;
}

/**
 * @brief Teste si une ligne est acceptee ou refusee par validate_syntax.
 */
static void	test_syntax_case(char *input, int expected)
{
	t_shell	shell;
	int		result;

	init_test_shell(&shell);
	if (tokenizer(input, &shell))
	{
		result = 1;
		printf("[INFO] tokenizer error for: %s\n", input);
	}
	else
		result = validate_syntax(shell.token);
	if (result == expected)
		printf("[PASS]");
	else
		printf("[FAIL]");
	printf(" input: %-18s | result: %d | expected: %d\n",
		input, result, expected);
	free_tokens(shell.token);
}

/**
 * @brief Lance les cas valides et invalides de syntaxe.
 */
int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	(void)env;
	printf("\n=== SYNTAX VALID CASES ===\n");
	test_syntax_case("echo hello", 0);
	test_syntax_case("echo hello | wc", 0);
	test_syntax_case("cat < infile", 0);
	test_syntax_case("echo hi > outfile", 0);
	test_syntax_case("cat << EOF", 0);
	test_syntax_case("echo hi >> outfile", 0);
	printf("\n=== SYNTAX INVALID CASES ===\n");
	test_syntax_case("|", 1);
	test_syntax_case("| echo", 1);
	test_syntax_case("echo |", 1);
	test_syntax_case("echo || wc", 1);
	test_syntax_case("echo >", 1);
	test_syntax_case("echo <", 1);
	test_syntax_case("echo >>", 1);
	test_syntax_case("echo <<", 1);
	test_syntax_case("echo > |", 1);
	test_syntax_case("echo < >", 1);
	return (0);
}
