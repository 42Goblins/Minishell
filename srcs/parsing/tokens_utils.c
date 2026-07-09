/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 02:43:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/07/09 02:44:47 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Create one token node with its type, value and next set to NULL.
 */
t_token	*new_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	if (value)
	{
		token->value = ft_strdup(value);
		if (!token->value)
		{
			free(token);
			return (NULL);
		}
	}
	else
		token->value = NULL;
	token->next = NULL;
	return (token);
}

/**
 * @brief Add a token node at the end of the token linked list.
 */
void	add_token_back(t_token **tokens, t_token *new)
{
	t_token	*current;

	if (!tokens || !new)
		return ;
	if (!*tokens)
	{
		*tokens = new;
		return ;
	}
	current = *tokens;
	while (current->next)
		current = current->next;
	current->next = new;
}
