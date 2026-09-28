/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_options.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 04:10:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/28 04:10:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if an export option is supported by bash.
 */
static int	is_export_option(char *arg)
{
	if (ft_strcmp(arg, "-n") == 0 || ft_strcmp(arg, "-a") == 0
		|| ft_strcmp(arg, "-p") == 0)
		return (1);
	return (0);
}

/**
 * @brief Updates export option flags used by exec_export.
 */
static void	set_export_option_flags(char *arg, int *print_list,
	int *silent_empty)
{
	if (ft_strcmp(arg, "-p") == 0 || ft_strcmp(arg, "-n") == 0)
		*print_list = 1;
	if (ft_strcmp(arg, "-a") == 0)
		*silent_empty = 1;
}

/**
 * @brief Skips supported export options and returns the first operand index.
 */
int	get_export_arg_start(char **cmd, int *print_list, int *silent_empty)
{
	int	i;

	i = 1;
	*print_list = 0;
	*silent_empty = 0;
	while (cmd[i] && cmd[i][0] == '-' && cmd[i][1] != '\0')
	{
		if (ft_strcmp(cmd[i], "--") == 0)
			return (i + 1);
		if (!is_export_option(cmd[i]))
		{
			ft_putstr_fd("minishell: export: ", STDERR_FILENO);
			ft_putstr_fd(cmd[i], STDERR_FILENO);
			ft_putstr_fd(": invalid option\n", STDERR_FILENO);
			return (-1);
		}
		set_export_option_flags(cmd[i], print_list, silent_empty);
		i++;
	}
	return (i);
}
