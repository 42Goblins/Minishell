/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/29 01:45:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/03 16:56:48 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
 * @brief Returns the length of the word starting at the given index.
 */
int	word_length(char *input, int i)
{
	int	length;

	length = 0;
	while (input[i + length] && !is_blank(input[i + length])
		&& input[i + length] != '|' && input[i + length] != '<'
		&& input[i + length] != '>')
		length++;
	return (length);
}
