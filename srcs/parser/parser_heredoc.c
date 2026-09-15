/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 18:49:11 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/16 01:54:19 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	fill_heredoc_pipe(int write_fd, char *delimiter);
static int	write_heredoc_line(int write_fd, char *line);
static void	print_heredoc_eof_warning(char *delimiter);

int	open_heredoc_redirection(t_cmd *cmd, char *delimiter)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
		return (perror("pipe"), 1);
	if (fill_heredoc_pipe(pipefd[1], delimiter) != 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	close(pipefd[1]);
	if (cmd->fd_in != 0)
		close(cmd->fd_in);
	cmd->fd_in = pipefd[0];
	return (0);
}

static int	fill_heredoc_pipe(int write_fd, char *delimiter)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (line == NULL)
			return (print_heredoc_eof_warning(delimiter), 0);
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			return (0);
		}
		if (write_heredoc_line(write_fd, line) != 0)
		{
			free(line);
			return (1);
		}
		free(line);
	}
}

static int	write_heredoc_line(int write_fd, char *line)
{
	if (write(write_fd, line, ft_strlen(line)) == -1)
	{
		perror("write");
		return (1);
	}
	if (write(write_fd, "\n", 1) == -1)
	{
		perror("write");
		return (1);
	}
	return (0);
}

static void	print_heredoc_eof_warning(char *delimiter)
{
	ft_putstr_fd("minishell: warning: here-document delimited by ",
	STDERR_FILENO);
	ft_putstr_fd("end-of-file (wanted `", STDERR_FILENO);
	ft_putstr_fd(delimiter, STDERR_FILENO);
	ft_putstr_fd("')\n", STDERR_FILENO);
}