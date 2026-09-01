/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 04:45:51 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/01 02:47:42 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	validate_pipe(t_token *current);
static int	validate_redirection(t_token *current);

/**
 * @brief Checks if the token list contains syntax errors.
 */
int	validate_syntax(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		if (validate_pipe(current))
			return (1);
		if (validate_redirection(current))
			return (1);
		current = current->next;
	}
	return (0);
}

/**
 * @brief Checks if a pipe token is correctly placed.
 */
static int	validate_pipe(t_token *current)
{
	if (!current)
		return (1);
	if (current->type != T_PIPE)
		return (0);
	if (!current->prev)
		return (1);
	if (current->next == NULL)
		return (1);
	if (current->next->type == T_PIPE)
		return (1);
	return (0);
}

/**
 * @brief Checks if a redirection token is followed by a word.
 */
static int	validate_redirection(t_token *current)
{
	if (!current)
		return (1);
	if (!is_redirection_token(current->type))
		return (0);
	if (!current->next)
		return (1);
	if (current->next->type != T_WORD)
		return (1);
	return (0);
}
