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

/*
 * Ce fichier teste le lexer/tokenizer seul.
 *
 * Il sert a verifier que la ligne est decoupee en bons tokens, que les quotes
 * restent dans token->value au bon moment, que had_quotes est pose, et que les
 * erreurs de quotes non fermees sont bien detectees.
 *
 * Le retrait des quotes est aussi teste ici, mais dans la vraie pipeline il
 * doit rester appele apres l'expansion, pas directement dans tokenizer.
 */

/**
 * @brief Affiche la liste de tokens pour verifier le tokenizer.
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

/**
 * @brief Teste une ligne valide et affiche les tokens obtenus.
 */
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

/**
 * @brief Verifie qu'une ligne invalide fait bien echouer le tokenizer.
 */
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
 * @brief Teste le retrait des quotes sur une seule string.
 */
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

/**
 * @brief Teste le retrait des quotes sur une liste de tokens.
 */
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
 * @brief Lance les tests temporaires du tokenizer sans toucher au vrai main.
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
