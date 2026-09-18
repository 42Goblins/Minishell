/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_expansion.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 22:46:29 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/17 03:20:54 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Ce fichier teste l'expansion sans passer par le vrai minishell.
 *
 * Il verifie les cas importants de $VAR, $?, $digit, variable absente, single
 * quotes, double quotes, variables collees, et heredoc delimiter non expande.
 * L'environnement est fake pour garder des resultats previsibles.
 */

/**
 * @brief Compare deux resultats de test, en gerant aussi le cas NULL.
 */
static bool	values_match(char *value, char *expected)
{
	if (!value && !expected)
		return (true);
	if (!value || !expected)
		return (false);
	return (ft_strcmp(value, expected) == 0);
}

/**
 * @brief Cree un petit environnement fake pour tester l'expansion.
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
 * @brief Teste l'assemblage de deux morceaux alloues de l'expansion.
 */
static void	test_append_part(char *first, char *second, char *expected)
{
	char	*result;

	result = append_expansion_part(ft_strdup(first), ft_strdup(second));
	if (values_match(result, expected))
		printf("[PASS] ");
	else
		printf("[FAIL] ");
	printf("\"%s\" + \"%s\" | result: \"%s\" | expected: \"%s\"\n",
		first, second, result ? result : "(NULL)", expected);
	free(result);
}

/**
 * @brief Lance plusieurs tests sur append_expansion_part.
 */
static void	test_append_parts(void)
{
	printf("\n=== APPEND EXPANSION PARTS ===\n");
	test_append_part("hello ", "chloe", "hello chloe");
	test_append_part("", "test", "test");
	test_append_part("abc", "", "abc");
	test_append_part("$", "USER", "$USER");
}

/**
 * @brief Teste si un dollar donne bien une expansion ou non.
 */
static void	test_one_dollar(char *word, int i, bool in_single, bool expected)
{
	bool	result;

	result = is_dollar_expand(word, i, in_single);
	if (result == expected)
		printf("[PASS] ");
	else
		printf("[FAIL] ");
	printf("word: %-10s | i: %2d | single: %d | result: %d | expected: %d\n",
		word ? word : "(NULL)", i, in_single, result, expected);
}

/**
 * @brief Teste les regles qui disent si un dollar est expandable.
 */
static void	test_dollar_expansion(void)
{
	printf("\n=== DOLLAR EXPANSION ===\n");
	test_one_dollar("$USER", 0, false, true);
	test_one_dollar("abc$USER", 3, false, true);
	test_one_dollar("$USER", 0, true, false);
	test_one_dollar("$?", 0, false, true);
	test_one_dollar("$?", 0, true, false);
	test_one_dollar("$2USER", 0, false, true);
	test_one_dollar("$2USER", 0, true, false);
	test_one_dollar("$", 0, false, false);
	test_one_dollar("USER", 0, false, false);
	test_one_dollar(NULL, 0, false, false);
	test_one_dollar("$USER", -1, false, false);
}

/**
 * @brief Teste la valeur recuperee pour un nom de variable apres le dollar.
 */
static void	test_get_var_value(char *input, t_env *env, char *expected)
{
	char	*value;

	value = get_var_value(input, env);
	if (values_match(value, expected))
		printf("[PASS] ");
	else
		printf("[FAIL] ");
	printf("after $: %-10s | result: %-12s | expected: %s\n",
		input ? input : "(NULL)", value ? value : "(NULL)",
		expected ? expected : "(NULL)");
	free(value);
}

/**
 * @brief Teste la longueur du nom de variable situe apres un dollar.
 */
static void	test_one_var_len(char *input, int expected)
{
	int	result;

	result = var_name_len(input);
	if (result == expected)
		printf("[PASS] ");
	else
		printf("[FAIL] ");
	printf("input: %-10s | result: %d | expected: %d\n",
		input ? input : "(NULL)", result, expected);
}

/**
 * @brief Teste la recherche de valeurs dans un environnement controle.
 */
static void	test_get_var_values(void)
{
	t_env	home;
	t_env	user;

	init_test_env(&user, &home);
	*get_status() = 42;
	printf("\n=== VARIABLE VALUES ===\n");
	test_get_var_value("USER/test", &user, "chloe");
	test_get_var_value("HOME!", &user, "/home/chloe");
	test_get_var_value("?", &user, "42");
	test_get_var_value("MISSING", &user, "");
	test_get_var_value("2USER", &user, "");
	test_get_var_value("", &user, NULL);
	test_get_var_value(NULL, &user, NULL);
}

/**
 * @brief Teste les longueurs de noms de variables valides ou invalides.
 */
static void	test_var_name_len(void)
{
	printf("=== VARIABLE NAME LENGTHS ===\n");
	test_one_var_len("USER", 4);
	test_one_var_len("USER/test", 4);
	test_one_var_len("USER2!", 5);
	test_one_var_len("_NAME=", 5);
	test_one_var_len("2USER", 0);
	test_one_var_len("-USER", 0);
	test_one_var_len("", 0);
	test_one_var_len(NULL, 0);
}

/**
 * @brief Teste le resultat final retourne par expand_word.
 */
static void	test_expand_scan(char *word, t_env *env, char *expected)
{
	char	*result;

	result = expand_word(word, env);
	if (values_match(result, expected))
		printf("[PASS] ");
	else
		printf("[FAIL] ");
	printf("word: %-15s | result: %-15s | expected: %s\n",
		word ? word : "(NULL)", result ? result : "(NULL)",
		expected ? expected : "(NULL)");
	free(result);
}

/**
 * @brief Teste expand_word avec des mots quotes et non quotes.
 */
static void	test_expand_scans(void)
{
	t_env	home;
	t_env	user;

	init_test_env(&user, &home);
	*get_status() = 127;
	printf("\n=== QUOTE-AWARE WORD SCAN ===\n");
	test_expand_scan("$USER", &user, "chloe");
	test_expand_scan("'$USER'", &user, "'$USER'");
	test_expand_scan("\"$USER\"", &user, "\"chloe\"");
	test_expand_scan("abc$USER", &user, "abcchloe");
	test_expand_scan("\"it's $USER\"", &user, "\"it's chloe\"");
	test_expand_scan("'\"$USER\"'", &user, "'\"$USER\"'");
	test_expand_scan("$?", &user, "127");
	test_expand_scan("status:$?", &user, "status:127");
	test_expand_scan("\"$?\"", &user, "\"127\"");
	test_expand_scan("'$?'", &user, "'$?'");
	test_expand_scan("$?$USER", &user, "127chloe");
	test_expand_scan("$MISSING", &user, "");
	test_expand_scan("a$MISSINGb", &user, "a");
	test_expand_scan("$USER$HOME", &user, "chloe/home/chloe");
	test_expand_scan("$?abc", &user, "127abc");
	test_expand_scan("$USER?", &user, "chloe?");
	test_expand_scan("\"$USER$?\"", &user, "\"chloe127\"");
	test_expand_scan("'$USER'$HOME", &user, "'$USER'/home/chloe");
	test_expand_scan("$2USER", &user, "USER");
	test_expand_scan("$12USER", &user, "2USER");
	test_expand_scan("$9abc", &user, "abc");
	test_expand_scan("$1", &user, "");
}

/**
 * @brief Compare les values des tokens avec les values attendues.
 */
static bool	token_values_match(t_token *tokens, char **expected)
{
	t_token	*current;
	int		i;

	current = tokens;
	i = 0;
	while (current && expected[i])
	{
		if (!values_match(current->value, expected[i]))
			return (false);
		current = current->next;
		i++;
	}
	return (current == NULL && expected[i] == NULL);
}

/**
 * @brief Teste expand_tokens sur une liste produite par le tokenizer.
 */
static void	test_expand_tokens(char *input, t_env *env, char **expected)
{
	t_shell	shell;

	shell.token = NULL;
	if (tokenizer(input, &shell))
	{
		printf("[FAIL] tokenizer: %s\n", input);
		return ;
	}
	if (expand_tokens(shell.token, env))
	{
		printf("[FAIL] expand_tokens: %s\n", input);
		free_tokens(shell.token);
		return ;
	}
	if (token_values_match(shell.token, expected))
		printf("[PASS] ");
	else
		printf("[FAIL] ");
	printf("expand tokens: %s\n", input);
	free_tokens(shell.token);
}

/**
 * @brief Teste expand_tokens puis remove_quotes_from_tokens.
 */
static void	test_expand_and_remove_quotes(char *input, t_env *env,
	char **expected)
{
	t_shell	shell;

	shell.token = NULL;
	if (tokenizer(input, &shell))
	{
		printf("[FAIL] tokenizer: %s\n", input);
		return ;
	}
	if (expand_tokens(shell.token, env))
	{
		printf("[FAIL] expand_tokens: %s\n", input);
		free_tokens(shell.token);
		return ;
	}
	if (remove_quotes_from_tokens(shell.token))
	{
		printf("[FAIL] remove_quotes_from_tokens: %s\n", input);
		free_tokens(shell.token);
		return ;
	}
	if (token_values_match(shell.token, expected))
		printf("[PASS] ");
	else
		printf("[FAIL] ");
	printf("expand + remove quotes: %s\n", input);
	free_tokens(shell.token);
}

/**
 * @brief Teste l'expansion sur une liste de tokens, sans retirer les quotes.
 */
static void	test_expand_token_lists(void)
{
	t_env	home;
	t_env	user;
	char	*expanded_args[] = {"echo", "\"chloe\"", "'$USER'", "127", NULL};
	char	*expanded_heredoc[] = {"cat", "<<", "\"$USER\"", NULL};
	char	*clean_args[] = {"echo", "chloe", "$USER", "127", NULL};
	char	*clean_mixed[] = {"echo", "$USER/home/chloe", NULL};
	char	*clean_heredoc[] = {"cat", "<<", "$USER", NULL};

	init_test_env(&user, &home);
	*get_status() = 127;
	printf("\n=== EXPAND TOKENS ===\n");
	test_expand_tokens("echo \"$USER\" '$USER' $?", &user, expanded_args);
	test_expand_tokens("cat << \"$USER\"", &user, expanded_heredoc);
	printf("\n=== EXPAND THEN REMOVE QUOTES ===\n");
	test_expand_and_remove_quotes("echo \"$USER\" '$USER' $?", &user,
		clean_args);
	test_expand_and_remove_quotes("echo '$USER'$HOME", &user, clean_mixed);
	test_expand_and_remove_quotes("cat << \"$USER\"", &user, clean_heredoc);
}

/**
 * @brief Lance tous les tests temporaires de la partie expansion.
 */
int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	(void)env;
	test_var_name_len();
	test_get_var_values();
	test_dollar_expansion();
	test_expand_scans();
	test_expand_token_lists();
	test_append_parts();
	return (0);
}
