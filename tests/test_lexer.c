/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_lexer.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/29 01:10:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/03 19:58:08 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Temporarily tests the first tokenizer version in isolation (to not modify main.c)
 */
int	main(int ac, char **av, char **env)
{
	t_shell	shell;

	(void)ac;
	(void)av;
	(void)env;
	ft_bzero(&shell, sizeof(t_shell));
	if (tokenizer("echo hello>>output", &shell))
		return (1);
	print_tokens(shell.token);
	free_tokens(shell.token);
	return (0);
}
