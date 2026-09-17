/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_entry.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 00:00:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/17 00:00:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * This file keeps the current parser entry point used by the main loop.
 */

/**
 * @brief Parses tokens without env for the current main loop.
 *
 * Tests and future main integration can use parse_tokens_with_env when heredoc
 * content needs access to the environment.
 */
t_cmd	*parse_tokens(t_token *tokens)
{
	return (parse_tokens_with_env(tokens, NULL));
}
