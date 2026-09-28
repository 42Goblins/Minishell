/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 03:47:59 by dgeara            #+#    #+#             */
/*   Updated: 2026/07/29 18:45:19 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks whether pwd received an unsupported option.
 */
static int	invalid_pwd_option(char *arg)
{
	if (!arg || arg[0] != '-' || arg[1] == '\0')
		return (0);
	if (ft_strcmp(arg, "--") == 0)
		return (0);
	ft_putstr_fd("minishell: pwd: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd(": invalid option\n", STDERR_FILENO);
	return (1);
}

/**
 * @brief Prints the current working directory after option validation.
 */
int	exec_pwd(char **cmd)
{
	char	*pwd;

	if (invalid_pwd_option(cmd[1]))
		return (2);
	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (perror("getcwd :"), 1);
	ft_putstr_fd(pwd, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
	free(pwd);
	return (0);
}
