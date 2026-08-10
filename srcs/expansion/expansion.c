/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 23:25:04 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/10 17:21:24 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*join_three_parts(char *first, char *second, char *third);
static char	*replace_current_var(char *result, int i, t_env *env, int *new_i);
static void	free_three_strings(char *first, char *second, char *third);

/**
 * @brief Appends an allocated part to the result and frees both strings.
 */
char	*append_expansion_part(char *built, char *part)
{
	char	*joined;

	if (!built || !part)
	{
		free(built);
		free(part);
		return (NULL);
	}
	joined = ft_strjoin(built, part);
	free(built);
	free(part);
	return (joined);
}

/**
 * @brief Expands variables in a word while respecting quote rules.
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
		if (result[i] == '\'' && !in_double)
			in_single = !in_single;
		else if (result[i] == '"' && !in_single)
			in_double = !in_double;
		else if (is_dollar_expand(result, i, in_single))
		{
			result = replace_current_var(result, i, env, &new_i);
			if (!result)
				return (NULL);
			i = new_i;
			continue ;	
			}
		i++;
	}
	return (result);
}

static char	*replace_current_var(char *result, int i, t_env *env, int *new_i)
{
	int		var_len;
	char	*before;
	char	*value;
	char	*after;
	char	*new_result;

	var_len = var_name_len(&result[i + 1]);
	before = ft_substr(result, 0, i);
	value = get_var_value(&result[i + 1], env);
	after = ft_substr(result, i + 1 + var_len,
			ft_strlen(result) - (i + 1 + var_len));
	if (!before || !value || !after)
	{
		free_three_strings(before, value, after);
		free(result);
		return (NULL);
	}
	*new_i = i + ft_strlen(value);
	new_result = join_three_parts(before, value, after);
	if (!new_result)
	{
		free(result);
		return (NULL);
	}
	free(result);
	return (new_result);
}

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

static void	free_three_strings(char *first, char *second, char *third)
{
	free(first);
	free(second);
	free(third);
}
