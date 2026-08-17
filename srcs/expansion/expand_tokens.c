/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 17:48:47 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/16 19:03:50 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	expand_tokens(t_token *tokens, t_env *env)
{
	t_token	*current;
	char	*old_value;
	char	*new_value;

	current = tokens;
	while (current)
	{
		if (current->type == T_WORD
			&& (current->prev == NULL || current->prev->type != T_HEREDOC))
		{
			old_value = current->value;
			new_value = expand_word(old_value, env);
			if (!new_value)
				return (1);
			free(old_value);
			current->value = new_value;
		}
		current = current->next;
	}
	return (0);
}
