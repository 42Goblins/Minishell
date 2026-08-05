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

static int	quoted_word_length(char *input, int i, char quote);

/**
 * @brief Returns the length of the word starting at the given index.
 */
int	word_length(char *input, int i)
{
	int	length;
	int	quote_length;

	length = 0;
	quote_length = 0;
	while (input[i + length] && !is_blank(input[i + length])
		&& input[i + length] != '|' && input[i + length] != '<'
		&& input[i + length] != '>')
	{
		if (input[i + length] == '\'' || input[i + length] == '"')
		{
			quote_length = quoted_word_length(input, i + length,
					input[i + length]);
			if (quote_length == -1)
				return (-1);
			length += quote_length;
		}
		else
			length++;
	}
	return (length);
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
static int	quoted_word_length(char *input, int i, char quote)
{
	int	length;

	length = 1;
	while (input[i + length] && input[i + length] != quote)
		length++;
	if (input[i + length] == quote)
		length++;
	else
		return (-1);
	return (length);
}
