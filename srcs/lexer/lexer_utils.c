/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/29 01:45:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/05 17:28:42 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	quoted_word_len(char *input, int i, char quote);

/**
 * @brief Returns the length of the word starting at the given index.
 */
int	word_len(char *input, int i)
{
	int	len;
	int	quote_len;

	len = 0;
	quote_len = 0;
	while (input[i + len] && !is_blank(input[i + len])
		&& input[i + len] != '|' && input[i + len] != '<'
		&& input[i + len] != '>')
	{
		if (input[i + len] == '\'' || input[i + len] == '"')
		{
			quote_len = quoted_word_len(input, i + len, input[i + len]);
			if (quote_len == -1)
				return (-1);
			len += quote_len;
		}
		else
			len++;
	}
	return (len);
}

/**
 * @brief Checks whether a character is a blank.
 */
int	is_blank(char character)
{
	if (character == ' ' || character == '\t')
		return (1);
	return (0);
}

/**
 * @brief Returns the length from an opening quote to its closing quote.
 */
static int	quoted_word_len(char *input, int i, char quote)
{
	int	len;

	len = 1;
	while (input[i + len] && input[i + len] != quote)
		len++;
	if (input[i + len] == quote)
		len++;
	else
		return (-1);
	return (len);
}
