/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 23:25:04 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/10 01:56:39 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*join_three_parts(char *first, char *second, char *third);
static char	*replace_current_var(char *result, int i, t_env *env, int *new_i);

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
	(void)env;
	if (!word)
		return (NULL);
	/*
	 * TODO prochaine etape:
	 * 1. partir de result = ft_strdup(word);
	 * 2. parcourir result en gardant l'etat single/double quotes;
	 * 3. si un $VAR est expandable, appeler replace_current_var;
	 * 4. reprendre le scan a new_i apres le remplacement.
	 */
	return (ft_strdup(word));
}

/*
 * TODO WIP:
 * Ce helper prepare le remplacement d'un $VAR.
 * Prochaine etape: sortir le cleanup d'erreur d'allocation
 * dans un petit helper, puis appeler cette fonction depuis expand_word.
 */
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
		free(before);
		free(value);
		free(after);
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
		free(first);
		free(second);
		free(third);
		return (NULL);
	}
	joined = ft_strjoin(tmp, third);
	free(tmp);
	free(first);
	free(second);
	free(third);
	return (joined);
}
