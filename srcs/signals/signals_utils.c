/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 00:00:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/24 00:00:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Prints the visible message for a child signal.
 */
void	print_signal_message(int signal)
{
	if (signal == SIGINT)
		write(1, "\n", 1);
	else if (signal == SIGQUIT)
		ft_putstr_fd("Quit (core dumped)\n", STDERR_FILENO);
}

/**
 * @brief Records whether a child exited because of SIGINT or SIGQUIT.
 */
void	track_child_signal(int status, int *sigint, int *sigquit)
{
	if (!WIFSIGNALED(status))
		return ;
	if (WTERMSIG(status) == SIGINT)
		*sigint = 1;
	else if (WTERMSIG(status) == SIGQUIT)
		*sigquit = 1;
}

/**
 * @brief Prints one signal message after all pipeline children are reaped.
 */
void	print_pipeline_signal(int sigint, int sigquit)
{
	if (sigint)
		print_signal_message(SIGINT);
	else if (sigquit)
		print_signal_message(SIGQUIT);
}
