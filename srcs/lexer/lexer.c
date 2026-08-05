/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/29 01:10:43 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/05 19:16:34 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	tokenize_current_char(char *input, int i, t_shell *shell);
static int	tokenizer_error(t_shell *shell);
static int	add_word_token(char *input, int start, int length, t_shell *shell);

/**
 * @brief Tokenizes a line into a linked list of tokens.
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
		length = tokenize_current_char(input, i, shell);
		if (length == -1)
			return (tokenizer_error(shell));
		i += length;
	}
	return (0);
}

/**
 * @brief Adds the token at index and returns its consumed length.
 */
static int	tokenize_current_char(char *input, int i, t_shell *shell)
{
	int	length;

	if (input[i] == '|')
	{
		if (add_operator_token(shell, T_PIPE, "|"))
			return (-1);
		return (1);
	}
	if (input[i] == '<')
		return (add_redir_in_or_heredoc(input, i, shell));
	if (input[i] == '>')
		return (add_redir_out_or_append(input, i, shell));
	length = word_length(input, i);
	if (length == -1)
		return (-1);
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
 * @brief Creates an operator token and adds it to the token list.
 */
int	add_operator_token(t_shell *shell, t_token_type type, char *str)
{
	t_token	*new_token;
	char	*value;

	value = ft_strdup(str);
	if (!value)
		return (1);
	new_token = create_token_node(type, value);
	if (!new_token)
		return (free(value), 1);
	add_token_back(&shell->token, new_token);
	return (0);
}

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
	if (ft_strchr(value, '\'') || ft_strchr(value, '"'))
		new_token->had_quotes = true;
	add_token_back(&shell->token, new_token);
	return (0);
}
