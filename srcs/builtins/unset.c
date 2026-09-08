/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/28 02:32:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/08 03:44:28 by dgeara           ###   ########.fr       */
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
			if (ft_strcmp(current->key, cmd[i]))
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
