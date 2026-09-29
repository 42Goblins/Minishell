/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 18:49:11 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/29 04:18:47 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * This file handles heredoc redirections.
 * It reads heredoc input, stores it in a pipe, and gives the read fd to cmd.
 */

static void	heredoc_child(t_shell *shell, t_cmd *cmd,
				int pipefd[2], t_token *delimiter);
static int	wait_heredoc_child(pid_t pid, int pipefd[2]);
static int	read_heredoc(int write_fd, char *delimiter,
				bool should_expand, t_env *env);
static void	print_heredoc_eof_warning(char *delimiter);

/**
 * @brief Reads a heredoc in a child process and stores its input fd.
 *
 * The child fills the write side of a pipe. The parent keeps the read side in
 * cmd->fd_in when heredoc collection succeeds.
 */
int	open_heredoc_redirection(t_shell *shell, t_cmd *cmd,
	t_token *delimiter)
{
	int		pipefd[2];
	pid_t	pid;

	if (cmd == NULL || delimiter == NULL || delimiter->value == NULL)
		return (1);
	if (pipe(pipefd) == -1)
		return (perror("pipe"), 1);
	ignore_exec_signals();
	pid = fork();
	if (pid == -1)
	{
		setup_signals();
		close(pipefd[0]);
		close(pipefd[1]);
		return (perror("fork"), 1);
	}
	if (pid == 0)
		heredoc_child(shell, cmd, pipefd, delimiter);
	if (wait_heredoc_child(pid, pipefd) != 0)
		return (1);
	if (cmd->fd_in != 0)
		close(cmd->fd_in);
	cmd->fd_in = pipefd[0];
	return (0);
}

/**
 * @brief Reads heredoc content in the child process and exits with its status.
 */
static void	heredoc_child(t_shell *shell, t_cmd *cmd,
		int pipefd[2], t_token *delimiter)
{
	int	res;

	setup_heredoc_signals();
	close(pipefd[0]);
	res = read_heredoc(pipefd[1], delimiter->value,
			!delimiter->had_quotes, shell->env);
	close(pipefd[1]);
	free_tab(cmd->cmd_and_args);
	free(cmd);
	clean_exit(shell, res);
}

/**
 * @brief Waits for the heredoc child and cancels on interruption.
 *
 * A Ctrl-C can appear as exit(130) from the child handler or as a SIGINT
 * termination. Both cases close the read end and set the shell status to 130.
 */
static int	wait_heredoc_child(pid_t pid, int pipefd[2])
{
	int	status;
	int	code;

	close(pipefd[1]);
	waitpid(pid, &status, 0);
	setup_signals();
	if (WIFEXITED(status))
	{
		code = WEXITSTATUS(status);
		if (code != 0)
		{
			if (code == 130)
				*get_status() = 130;
			close(pipefd[0]);
			return (1);
		}
	}
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		*get_status() = 130;
		close(pipefd[0]);
		return (1);
	}
	return (0);
}

/**
 * @brief Reads heredoc lines and writes them to the pipe.
 *
 * Each line is optionally expanded. EOF prints a warning, while SIGINT returns
 * 130 so the parent can cancel the command.
 */
static int	read_heredoc(int write_fd, char *delimiter, bool should_expand,
	t_env *env)
{
	char	*line;

	while (1)
	{
		line = read_input("> ");
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
