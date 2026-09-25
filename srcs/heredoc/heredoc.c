/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 18:49:11 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/25 03:43:52 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * This file handles heredoc redirections.
 * It reads heredoc input, stores it in a pipe, and gives the read fd to cmd.
 */

static void	heredoc_child(int pipefd[2], t_token *delimiter, t_env *env);
static int	wait_heredoc_child(pid_t pid, int pipefd[2]);
static int	fill_heredoc_pipe(int write_fd, char *delimiter,
				bool should_expand, t_env *env);
static void	print_heredoc_eof_warning(char *delimiter);

/**
 * @brief Reads a heredoc and stores its input fd in cmd->fd_in.
 *
 * The delimiter token keeps both the delimiter value and the quote info.
 * If the delimiter was not quoted, heredoc content is expanded.
 */
int	open_heredoc_redirection(t_cmd *cmd, t_token *delimiter, t_env *env)
{
	int	pipefd[2];

	if (cmd == NULL || delimiter == NULL || delimiter->value == NULL)
		return (1);
	if (pipe(pipefd) == -1)
		return (perror("pipe"), 1);
	setup_heredoc_signals();
	if (fill_heredoc_pipe(pipefd[1], delimiter->value,
			!delimiter->had_quotes, env) != 0)
	{
		setup_signals();
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	setup_signals();
	close(pipefd[1]);
	if (cmd->fd_in != 0)
		close(cmd->fd_in);
	cmd->fd_in = pipefd[0];
	return (0);
}

void	heredoc_child(int pipefd[2], t_token *delimiter, t_env *env)
{
	
}

/**
 * @brief Reads heredoc lines until the delimiter or EOF is reached.
 *
 * Each line is optionally expanded, then written to the heredoc pipe.
 */
static int	fill_heredoc_pipe(int write_fd, char *delimiter, bool should_expand,
	t_env *env)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (line == NULL && g_signal == SIGINT)
			return (130);
		if (line == NULL)
			return (print_heredoc_eof_warning(delimiter), 0);
		if (ft_strcmp(line, delimiter) == 0)
			return (free(line), 0);
		if (write_heredoc_content(write_fd, line, should_expand, env) != 0)
		{
			free(line);
			return (1);
		}
		free(line);
	}
}

/**
 * @brief Prints the bash-like warning used when EOF ends a heredoc.
 */
static void	print_heredoc_eof_warning(char *delimiter)
{
	ft_putstr_fd("minishell: warning: here-document delimited by ",
		STDERR_FILENO);
	ft_putstr_fd("end-of-file (wanted `", STDERR_FILENO);
	ft_putstr_fd(delimiter, STDERR_FILENO);
	ft_putstr_fd("')\n", STDERR_FILENO);
}

