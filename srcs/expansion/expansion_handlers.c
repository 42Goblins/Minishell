/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_handlers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 21:54:51 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/08 23:53:29 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_var_char(char c);
static char	*get_var_name(char *var);

/**
 * @brief Checks whether a dollar sign starts an expandable variable.
 */
bool	is_dollar_expand(char *word, int i, bool in_single)
{
	if (!word || i < 0)
		return (false);
	if (word[i] != '$')
		return (false);
	if (in_single)
		return (false);
	return (var_name_len(&word[i + 1]) > 0);
}

/**
 * @brief Allocates the value of the variable found at the start of a string.
 */
char	*get_var_value(char *var, t_env *env)
{
	char	*name;
	char	*value;

	name = get_var_name(var);
	if (!name)
		return (NULL);
	value = get_env_value(env, name);
	free(name);
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}

/**
 * @brief Extracts and allocates the variable name at the start of a string.
 */
static char	*get_var_name(char *var)
{
	int	len;

	len = var_name_len(var);
	if (!len)
		return (NULL);
	return (ft_substr(var, 0, len));
}

/**
 * @brief Returns the length of a valid variable name.
 */
int	var_name_len(char *var)
{
	int	i;

	i = 0;
	if (!var || (!ft_isalpha(var[i]) && var[i] != '_'))
		return (0);
	while (var[i] && is_var_char(var[i]))
		i++;
	return (i);
}

/**
 * @brief Checks whether a character is alphanumeric or an underscore.
 */
static int	is_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}
