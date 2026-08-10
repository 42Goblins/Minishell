/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_expansion.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 22:46:29 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/08 22:59:57 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	test_one_dollar("$2USER", 0, false, false);
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
	printf("\n=== VARIABLE VALUES ===\n");
	test_get_var_value("USER/test", &user, "chloe");
	test_get_var_value("HOME!", &user, "/home/chloe");
	test_get_var_value("MISSING", &user, "");
	test_get_var_value("2USER", &user, NULL);
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
	printf("\n=== QUOTE-AWARE WORD SCAN ===\n");
	test_expand_scan("$USER", &user, "chloe");
	test_expand_scan("'$USER'", &user, "'$USER'");
	test_expand_scan("\"$USER\"", &user, "\"chloe\"");
	test_expand_scan("abc$USER", &user, "abcchloe");
	test_expand_scan("\"it's $USER\"", &user, "\"it's chloe\"");
	test_expand_scan("'\"$USER\"'", &user, "'\"$USER\"'");
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
	test_append_parts();
	return (0);
}
