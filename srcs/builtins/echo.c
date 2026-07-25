/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 01:39:54 by dgeara            #+#    #+#             */
/*   Updated: 2026/07/25 02:28:11 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	has_n_flag(char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] != '-' || str[1] != 'n')
		return (0);
	i = 2;
	while (str[i])
	{
		if (str[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

void	exec_echo(char **cmd)
{
	int	i;
	int	no_newline;

	i = 1;
	no_newline = 0;
	while (cmd[i] && has_n_flag(cmd[i]))
	{
		no_newline = 1;
		i++;
	}
	while (cmd[i])
	{
		ft_putstr_fd(cmd[i], STDOUT_FILENO);
		if (cmd[i + 1])
			ft_putstr_fd(" ", STDOUT_FILENO);
		i++;
	}
	if (!no_newline)
		ft_putstr_fd("\n", STDOUT_FILENO);
}
