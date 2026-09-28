/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 03:45:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/28 03:45:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks whether cd received an unsupported option.
 */
int	invalid_cd_option(char *arg)
{
	if (!arg || arg[0] != '-' || arg[1] == '\0')
		return (0);
	if (ft_strcmp(arg, "--") == 0 || ft_strcmp(arg, "-") == 0
		|| ft_strcmp(arg, "-L") == 0 || ft_strcmp(arg, "-P") == 0)
		return (0);
	ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd(": invalid option\n", STDERR_FILENO);
	return (1);
}

/**
 * @brief Skips supported cd options and returns the path argument index.
 */
int	get_cd_path_index(char **cmd)
{
	int	i;

	i = 1;
	while (cmd[i] && (ft_strcmp(cmd[i], "-L") == 0
			|| ft_strcmp(cmd[i], "-P") == 0))
		i++;
	return (i);
}
