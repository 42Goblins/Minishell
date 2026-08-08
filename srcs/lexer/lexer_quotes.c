/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 16:07:24 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/08 20:28:16 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	copy_without_quotes(char *value, char *stripped);

/**
 * @brief Removes active quotes from every quoted word token.
 */
int	remove_quotes_from_tokens(t_token *tokens)
{
	t_token	*current;
	char	*new_value;

	current = tokens;
	while (current)
	{
		if (current->type == T_WORD && current->had_quotes)
		{
			new_value = remove_quotes(current->value);
			if (!new_value)
				return (1);
			free(current->value);
			current->value = new_value;
		}
		current = current->next;
	}
	return (0);
}

/**
 * @brief Returns a new string with active quotes removed.
 */
char	*remove_quotes(char *value)
{
	char	*stripped;

	stripped = malloc(sizeof(char) * (ft_strlen(value) + 1));
	if (!stripped)
		return (NULL);
	copy_without_quotes(value, stripped);
	return (stripped);
}

/**
 * @brief Copies a word while removing its active quotes.
 */
static void	copy_without_quotes(char *value, char *stripped)
{
	int		i;
	int		j;
	bool	in_single;
	bool	in_double;

	i = 0;
	j = 0;
	in_single = false;
	in_double = false;
	while (value[i])
	{
		if (value[i] == '\'' && !in_double)
			in_single = !in_single;
		else if (value[i] == '"' && !in_single)
			in_double = !in_double;
		else
		{
			stripped[j] = value[i];
			j++;
		}
		i++;
	}
	stripped[j] = '\0';
}
