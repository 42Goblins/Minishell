/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 23:25:04 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/09 16:22:46 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * This file expands dollar expressions inside one word.
 * Quotes are kept here to know when $ must be expanded or ignored.
 */

static int	handle_expansion_char(char **result, int *i, t_env *env,
				bool in_single);
static char	*join_three_parts(char *first, char *second, char *third);
static char	*replace_current_var(char *result, int i, t_env *env, int *new_i);
static bool	update_quote_state(char c, bool *in_single, bool *in_double);

/**
 * @brief Expands the variables in one word.
 *
 * Quotes are still in the string here. The loop updates the quote state first,
 * then expands a `$` only if the current context allows it.
 */
char	*expand_word(char *word, t_env *env)
{
	int		i;
	char	*result;
	bool	in_single;
	bool	in_double;

	if (!word)
		return (NULL);
	result = ft_strdup(word);
	if (!result)
		return (NULL);
	i = 0;
	in_single = false;
	in_double = false;
	while (result[i])
	{
		if (update_quote_state(result[i], &in_single, &in_double))
			i++;
		else if (handle_expansion_char(&result, &i, env, in_single))
			return (NULL);
	}
	return (result);
}

/**
 * @brief Updates whether we are inside single or double quotes.
 */
static bool	update_quote_state(char c, bool *in_single, bool *in_double)
{
	if (c == '\'' && !*in_double)
	{
		*in_single = !*in_single;
		return (true);
	}
	if (c == '"' && !*in_single)
	{
		*in_double = !*in_double;
		return (true);
	}
	return (false);
}

/**
 * @brief Replaces the variable found at index i.
 *
 * `result` is cut around the `$`: text before it, expanded value, and text
 * after the variable name. The old string is freed and new_i is set after the
 * inserted value, so expand_word can keep scanning from the right place.
 */
static char	*replace_current_var(char *result, int i, t_env *env, int *new_i)
{
	int		var_len;
	char	*before;
	char	*value;
	char	*after;
	char	*new_result;

	if (result[i + 1] == '?' || ft_isdigit(result[i + 1]))
		var_len = 1;
	else
		var_len = var_name_len(&result[i + 1]);
	before = ft_substr(result, 0, i);
	value = get_var_value(&result[i + 1], env);
	after = ft_substr(result, i + 1 + var_len,
			ft_strlen(result) - (i + 1 + var_len));
	if (!before || !value || !after)
		return (free_three_strings(before, value, after), free(result), NULL);
	*new_i = i + ft_strlen(value);
	new_result = join_three_parts(before, value, after);
	if (!new_result)
		return (free(result), NULL);
	free(result);
	return (new_result);
}

/**
 * @brief Joins three strings and frees them.
 */
static char	*join_three_parts(char *first, char *second, char *third)
{
	char	*tmp;
	char	*joined;

	tmp = ft_strjoin(first, second);
	if (!tmp)
	{
		free_three_strings(first, second, third);
		return (NULL);
	}
	joined = ft_strjoin(tmp, third);
	free(tmp);
	free_three_strings(first, second, third);
	return (joined);
}

/**
 * @brief Handles one character that is not an active quote.
 *
 * If the character starts an expansion, it replaces it and updates i.
 * Otherwise it only moves i to the next character.
 */
static int	handle_expansion_char(char **result, int *i, t_env *env,
	bool in_single)
{
	int	new_i;

	if (is_dollar_expand(*result, *i, in_single) == false)
	{
		(*i)++;
		return (0);
	}
	*result = replace_current_var(*result, *i, env, &new_i);
	if (!*result)
		return (1);
	*i = new_i;
	return (0);
}
