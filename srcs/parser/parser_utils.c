/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 03:40:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/01 03:40:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a token type is a redirection.
 */
int	is_redirection_token(t_token_type type)
{
	if (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_APPEND || type == T_HEREDOC)
		return (1);
	return (0);
}

/**
 * @brief Checks if an expanded word disappeared without being quoted.
 */
int	is_empty_unquoted_word(t_token *token)
{
	if (token->type == T_WORD && token->value[0] == '\0'
		&& token->had_quotes == false)
		return (1);
	return (0);
}
