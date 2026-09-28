/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/28 02:32:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/28 18:22:21 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Removes the specified variables from the environment.
 */
void	del_env_variable(t_env **first, t_env *prev, t_env *current)
{
	if (prev)
		prev->next = current->next;
	else
		*first = current->next;
	free_t_env(current);
}

/**
 * @brief Removes the specified variables from the environment.
 */
int	exec_unset(t_env **env, char **cmd)
{
	int		i;
	t_env	*current;
	t_env	*prev;

	i = 1;
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
