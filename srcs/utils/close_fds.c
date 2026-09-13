/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close_fds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 03:53:55 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/08 19:57:55 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Closes file descriptor if valid and resets it to -1,
 * preventing an accidental double close.
 */
int	safe_close_fd(int *fd)
{
	int	ret;

	if (!fd || *fd < 0)
		return (1);
	ret = close(*fd);
	*fd = -1;
	return (ret);
}

/**
 * @brief Closes a fd (prev_fd) and both ends of pipe, if still open
 * use to save space in exec_pipeline.c (might not be usefull anymore)
 */
void	safe_close_all_fd(int *fd, int *pipefd)
{
	if (fd)
		safe_close_fd(fd);
	if (pipefd)
	{
		safe_close_fd(&pipefd[0]);
		safe_close_fd(&pipefd[1]);
	}
}
