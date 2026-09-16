/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 03:58:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/16 02:17:36 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	clean_exit(t_shell *shell, int status)
{
	free_cmds(shell->cmds);
	free_tokens(shell->token);
	free_lst_env(shell->env);
	clear_history();
	exit(status);
}

int	is_num(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	exec_exit(t_shell *shell, char **cmd)
{
	int	status;

	status = *get_status();
	ft_putstr_fd("exit\n", 2);
	if (cmd[1])
	{
		if (!is_num(cmd[1]))
		{
			ft_putstr_fd("minishell: exit: ", 2);
			ft_putstr_fd(cmd[1], 2);
			ft_putstr_fd(": numeric argument required\n", 2);
			clean_exit (shell, 2);
		}
		if (cmd[2])
		{
			ft_putstr_fd("minishell: exit: too many arguments\n", 2);
			return (1);
		}
		else
			status = ft_atoi(cmd[1]);
	}
	clean_exit(shell, (status % 256));
	return (0);
}
