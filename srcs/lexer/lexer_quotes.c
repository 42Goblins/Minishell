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

static int	stripped_length(char *value);

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
	int		i;
	int		j;
	int		length;
	bool	in_single;
	bool	in_double;
	char	*stripped;

	length = stripped_length(value);
	stripped = malloc(sizeof(char) * (length + 1));
	if (!stripped)
		return (NULL);
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
	return (stripped);
}

/**
 * @brief Returns the length of a word after removing active quotes.
 */
static int	stripped_length(char *value)
{
	int		i;
	int		length;
	bool	in_single;
	bool	in_double;

	i = 0;
	length = 0;
	in_single = false;
	in_double = false;
	while (value[i])
	{
		if (value[i] == '\'' && !in_double)
			in_single = !in_single;
		else if (value[i] == '"' && !in_single)
			in_double = !in_double;
		else
			length++;
		i++;
	}
	return (length);
}
