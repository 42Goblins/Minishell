/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 01:52:56 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/07 02:11:32 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	open_current_redirection(t_cmd *cmd, t_token *current);
static int	open_input_redirection(t_cmd *cmd, char *filename);
static int	open_output_redirection(t_cmd *cmd, char *filename, int flags);
static int	print_redirection_error(char *filename);

/**
 * @brief Opens redirection files for one command and stores their fds.
 *
 * The scan stops at the next pipe. If several redirections target the same
 * side, the last one replaces the previous fd.
 */
int	open_redirections(t_cmd *cmd, t_token *tokens)
{
	t_token	*current;

	current = tokens;
	if (!current)
		return (0);
	while (current && current->type != T_PIPE)
	{
		if (is_redirection_token(current->type))
		{
			if (open_current_redirection(cmd, current))
				return (1);
			current = current->next->next;
		}
		else
			current = current->next;
	}
	return (0);
}

/**
 * @brief Opens the redirection represented by the current token.
 *
 * Heredoc is skipped for now because it will be handled separately.
 */
static int	open_current_redirection(t_cmd *cmd, t_token *current)
{
	if (current->type == T_REDIR_IN)
		return (open_input_redirection(cmd, current->next->value));
	if (current->type == T_REDIR_OUT)
		return (open_output_redirection(cmd, current->next->value,
				O_WRONLY | O_CREAT | O_TRUNC));
	if (current->type == T_APPEND)
		return (open_output_redirection(cmd, current->next->value,
				O_WRONLY | O_CREAT | O_APPEND));
	return (0);
}

/**
 * @brief Opens an input file and replaces cmd->fd_in.
 */
static int	open_input_redirection(t_cmd *cmd, char *filename)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		return (print_redirection_error(filename));
	if (cmd->fd_in != 0)
		close(cmd->fd_in);
	cmd->fd_in = fd;
	return (0);
}

/**
 * @brief Opens an output file with flags and replaces cmd->fd_out.
 */
static int	open_output_redirection(t_cmd *cmd, char *filename, int flags)
{
	int	fd;

	fd = open(filename, flags, 0644);
	if (fd == -1)
		return (print_redirection_error(filename));
	if (cmd->fd_out != 1)
		close(cmd->fd_out);
	cmd->fd_out = fd;
	return (0);
}

/**
 * @brief Prints an open error for a redirection and sets status to 1.
 */
static int	print_redirection_error(char *filename)
{
	char	*message;

	message = ft_strjoin("minishell: ", filename);
	if (!message)
	{
		perror("minishell");
		*get_status() = 1;
		return (1);
	}
	perror(message);
	free(message);
	*get_status() = 1;
	return (1);
}
