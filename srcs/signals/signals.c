/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 22:21:32 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/28 00:05:56 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * This file installs the signal behavior used by the interactive shell,
 * command execution waits, and heredoc children. The g_signal global only
 * stores the received signal number, as required by the subject.
 */

int	g_signal;

static void	sigint_handler(int sig);
static void	sigint_heredoc_handler(int sig);

/**
 * @brief Ignores interactive signals while the parent waits for a child.
 */
int	ignore_exec_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	return (0);
}

/**
 * @brief Installs the normal interactive prompt signal behavior.
 */
int	setup_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
	return (0);
}

/**
 * @brief Installs heredoc child signal behavior.
 */
int	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = sigint_heredoc_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
	return (0);
}

/**
 * @brief Handles Ctrl-C inside the heredoc child by closing stdin.
 */
static void	sigint_heredoc_handler(int sig)
{
	g_signal = sig;
	close(STDIN_FILENO);
	write(1, "\n", 1);
}

/**
 * @brief Handles Ctrl-C at the main prompt and redraws readline.
 */
static void	sigint_handler(int sig)
{
	g_signal = sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}
