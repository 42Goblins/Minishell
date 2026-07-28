/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/29 01:10:43 by cmauley           #+#    #+#             */
/*   Updated: 2026/07/29 01:41:48 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Extracts one word and adds it to the token list.
 */
static int	add_word_token(char *input, int start, int length, t_shell *shell)
{
	char		*value;
	t_token		*new_token;

	value = ft_substr(input, start, length);
	if (!value)
		return (1);
	new_token = create_token_node(T_WORD, value);
	if (!new_token)
		return (free(value), 1);
	add_token_back(&shell->token, new_token);
	return (0);
}

/**
 * @brief Creates a pipe token and adds it to the token list.
 */
static int	add_pipe_token(t_shell *shell)
{
	t_token	*new_token;
	char	*value;

	value = ft_strdup("|");
	if (!value)
		return (1);
	new_token = create_token_node(T_PIPE, value);
	if (!new_token)
		return (free(value), 1);
	add_token_back(&shell->token, new_token);
	return (0);
}

/**
 * @brief Adds the token at index and returns its consumed length.
 */
static int	add_next_token(char *input, int i, t_shell *shell)
{
	int	length;

	if (input[i] == '|')
	{
		if (add_pipe_token(shell))
			return (-1);
		return (1);
	}
	length = word_length(input, i);
	if (add_word_token(input, i, length, shell))
		return (-1);
	return (length);
}

/**
 * @brief Clears partial tokens after a tokenizer allocation failure.
 */
static int	tokenizer_error(t_shell *shell)
{
	free_tokens(shell->token);
	shell->token = NULL;
	return (1);
}

/**
 * @brief Tokenizes a line containing words separated by blanks.
 */
int	tokenizer(char *input, t_shell *shell)
{
	int	i;
	int	length;

	if (!input || !shell)
		return (1);
	i = 0;
	while (input[i])
	{
		while (is_blank(input[i]))
			i++;
		if (!input[i])
			break ;
		length = add_next_token(input, i, shell);
		if (length == -1)
			return (tokenizer_error(shell));
		i += length;
	}
	return (0);
}
