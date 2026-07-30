/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 03:58:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/07/30 05:10:10 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_num(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	exec_exit(char **cmd)
{
	ft_putstr_fd("exit\n", 2);
	if (cmd[1])
	{
		if (!is_num(cmd[1]))
		{
			ft_putstr_fd("minishell: exit: ", 2);
			ft_putstr_fd(cmd[1], 2);
			ft_putstr_fd(": numeric argument required\n", 2);
			exit (2); // a modif avec clean
		}
		else if(cmd[2])
		{
			ft_putstr_fd("minishell: exit: too many arguments\n", 2);
			// need add get status ? et return 2 ??
		}
		else
			exit(ft_atoi(cmd[1])); //ici aussi need clean avant
	}
	exit(0); //remplacer par une fonction exit clean, et add code exit dinamique
	return (0);
}
