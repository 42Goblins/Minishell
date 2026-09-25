/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 17:06:54 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/25 17:07:45 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Writes a heredoc line after expanding it when needed.
 *
 * The original readline string is freed by the caller. If expansion creates a
 * new string, this function frees that expanded copy before returning.
 */
int	write_heredoc_content(int write_fd, char *line, bool should_expand,
	t_env *env)
{
	char	*expanded;
	int		ret;

	if (!should_expand || env == NULL)
		return (write_heredoc_line(write_fd, line));
	expanded = expand_word(line, env);
	if (!expanded)
		return (1);
	ret = write_heredoc_line(write_fd, expanded);
	free(expanded);
	return (ret);
}

/**
 * @brief Writes one heredoc line and its newline to the pipe.
 */
int	write_heredoc_line(int write_fd, char *line)
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