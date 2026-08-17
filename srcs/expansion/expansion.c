/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 23:25:04 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/13 16:05:11 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*join_three_parts(char *first, char *second, char *third);
static char	*replace_current_var(char *result, int i, t_env *env, int *new_i);
static bool	update_quote_state(char c, bool *in_single, bool *in_double);

/**
 * @brief Expands variables in a word while respecting quote rules.
 *
 * The loop does three things:
 * - updates quote states when it reads a quote character
 * - replaces an expandable dollar expression when allowed
 * - otherwise moves to the next character
 */
char	*expand_word(char *word, t_env *env)
{
	int		i;
	int		new_i;
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
		else if (is_dollar_expand(result, i, in_single))
		{
			result = replace_current_var(result, i, env, &new_i);
			if (!result)
				return (NULL);
			i = new_i;
			continue ;
		}
		else
			i++;
	}
	return (result);
}

/**
 * @brief Updates single and double quote states for the current character.
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
 * @brief Replaces one expansion found at index i in result.
 *
 * Splits result into three parts:
 * - before: everything before the dollar sign
 * - value: the expanded value of $VAR, $? or $digit
 * - after: everything after the consumed variable name
 *
 * The function joins those parts into a new string, frees the old result,
 * and stores in new_i the index where expand_word should resume scanning.
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
 * @brief Joins three allocated strings and frees them.
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
