/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 20:05:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/03 20:05:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Adds either an input redirection or heredoc token.
 */
int	add_redir_in_or_heredoc(char *input, int i, t_shell *shell)
{
	if (input[i + 1] == '<')
	{
		if (add_operator_token(shell, T_HEREDOC, "<<"))
			return (-1);
		return (2);
	}
	if (add_operator_token(shell, T_REDIR_IN, "<"))
		return (-1);
	return (1);
}

/**
 * @brief Adds either an output redirection or append token.
 */
int	add_redir_out_or_append(char *input, int i, t_shell *shell)
{
	if (input[i + 1] == '>')
	{
		if (add_operator_token(shell, T_APPEND, ">>"))
			return (-1);
		return (2);
	}
	if (add_operator_token(shell, T_REDIR_OUT, ">"))
		return (-1);
	return (1);
}
