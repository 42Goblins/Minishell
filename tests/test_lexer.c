/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_lexer.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/29 01:10:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/08 20:47:55 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// !!!!!!!!!!!!!!!!! FICHIER TEMPO DE TEST POUR EVITER DE TOUCHER MAIN !!!!!!!!!!!!!

/**
 * @brief Prints the token list for debugging.
 */
static void	print_tokens(t_token *head)
{
	t_token	*current;

	if (!head)
	{
		printf("(no tokens)\n");
		return ;
	}
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

static int	test_remove_quotes(char *input)
{
	char	*stripped;

	stripped = remove_quotes(input);
	if (!stripped)
		return (1);
	printf("\nremove_quotes input: %s\n", input);
	printf("remove_quotes output: %s\n", stripped);
	free(stripped);
	return (0);
}

static int	test_remove_quotes_from_tokens(char *input)
{
	t_shell	shell;

	ft_bzero(&shell, sizeof(t_shell));
	if (tokenizer(input, &shell))
		return (1);
	printf("\nbefore quote removal:\n");
	print_tokens(shell.token);
	if (remove_quotes_from_tokens(shell.token))
	{
		free_tokens(shell.token);
		return (1);
	}
	printf("after quote removal:\n");
	print_tokens(shell.token);
	free_tokens(shell.token);
	return (0);
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
	if (test_input(""))
		return (1);
	if (test_input("   \t  "))
		return (1);
	if (test_input("echo \"\""))
		return (1);
	if (test_input("echo ''"))
		return (1);
	if (test_input("echo a\"\"b"))
		return (1);
	if (test_input("echo \"a|b\""))
		return (1);
	if (test_input("echo 'a>b'"))
		return (1);
	if (test_input("\"echo hello\""))
		return (1);
	if (test_input("|||"))
		return (1);
	if (test_input("><"))
		return (1);
	if (test_invalid_input("echo \"hello"))
		return (1);
	if (test_invalid_input("echo 'hello"))
		return (1);
	if (test_invalid_input("echo \"\"\""))
		return (1);
	if (test_invalid_input("echo 'abc"))
		return (1);
	if (test_remove_quotes("\"hello world\""))
		return (1);
	if (test_remove_quotes("'hello world'"))
		return (1);
	if (test_remove_quotes("abc\"def\"ghi"))
		return (1);
	if (test_remove_quotes("\"'hello'\""))
		return (1);
	if (test_remove_quotes("'\"hello\"'"))
		return (1);
	if (test_remove_quotes_from_tokens("echo abc\"def\"ghi"))
		return (1);
	if (test_remove_quotes_from_tokens("echo \"\""))
		return (1);
	if (test_remove_quotes_from_tokens("echo \"a|b\""))
		return (1);
	return (0);
}
