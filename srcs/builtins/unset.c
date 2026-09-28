/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/28 02:32:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/15 23:09:49 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	del_env_variable(t_env **first, t_env *prev, t_env *current)
{
	if (prev)
		prev->next = current->next;
	else
		*first = current->next;
	free_t_env(current);
}

/**
 * @brief Checks if an unset option is supported by bash.
 */
static int	is_unset_option(char *arg)
{
	if (ft_strcmp(arg, "-n") == 0 || ft_strcmp(arg, "-f") == 0
		|| ft_strcmp(arg, "-v") == 0)
		return (1);
	return (0);
}

/**
 * @brief Skips supported unset options and returns the first variable index.
 */
static int	get_unset_arg_start(char **cmd)
{
	int	i;

	i = 1;
	while (cmd[i] && cmd[i][0] == '-' && cmd[i][1] != '\0')
	{
		if (ft_strcmp(cmd[i], "--") == 0)
			return (i + 1);
		if (!is_unset_option(cmd[i]))
		{
			ft_putstr_fd("minishell: unset: ", STDERR_FILENO);
			ft_putstr_fd(cmd[i], STDERR_FILENO);
			ft_putstr_fd(": invalid option\n", STDERR_FILENO);
			return (-1);
		}
		i++;
	}
	return (i);
}

/**
 * @brief Removes variables from the shell environment.
 */
int	exec_unset(t_env **env, char **cmd)
{
	int		i;
	t_env	*current;
	t_env	*prev;

	i = get_unset_arg_start(cmd);
	if (i == -1)
		return (2);
	while (cmd[i])
	{
		current = *env;
		prev = NULL;
		while (current)
		{
			if (ft_strcmp(current->key, cmd[i]) == 0)
			{
				del_env_variable(env, prev, current);
				break ;
			}
			prev = current;
			current = current->next;
		}
		i++;
	}
	return (0);
}
