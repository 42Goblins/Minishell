/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_lexer.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/29 01:10:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/05 19:20:38 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Prints the token list for debugging.
 */
static void	print_tokens(t_token *head)
{
	t_token	*current;

	current = head;
	while (current)
	{
		printf("type: %d, value: %s, had_quotes: %d\n",
			current->type, current->value, current->had_quotes);
		current = current->next;
	}
}

static int	test_input(char *input)
{
	t_shell	shell;

	ft_bzero(&shell, sizeof(t_shell));
	printf("\ninput: %s\n", input);
	if (tokenizer(input, &shell))
		return (1);
	print_tokens(shell.token);
	free_tokens(shell.token);
	return (0);
}

static int	test_invalid_input(char *input)
{
	t_shell	shell;

	ft_bzero(&shell, sizeof(t_shell));
	printf("\ninput: %s\n", input);
	if (tokenizer(input, &shell))
	{
		printf("tokenizer failed as expected\n");
		return (0);
	}
	else
	{
		print_tokens(shell.token);
		free_tokens(shell.token);
		return (1);
	}
}

/**
 * @brief Temporarily tests the first tokenizer version in isolation (to not modify main.c)
 */
int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	(void)env;
	if (test_input("echo hello"))
		return (1);
	if (test_input("echo hello|wc"))
		return (1);
	if (test_input("cat<input"))
		return (1);
	if (test_input("echo hello>output"))
		return (1);
	if (test_input("cat<<EOF"))
		return (1);
	if (test_input("echo hello>>output"))
		return (1);
	if (test_input("echo 'hello world'"))
		return (1);
	if (test_input("echo \"hello world\""))
		return (1);
	if (test_input("echo hello\" world\""))
		return (1);
	if (test_input("echo \"hello\"world"))
		return (1);
	if (test_input("echo abc\"def\"ghi"))
		return (1);
	if (test_invalid_input("echo \"hello"))
		return (1);
	if (test_invalid_input("echo 'hello"))
		return (1);
	return (0);
}
