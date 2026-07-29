/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/28 02:32:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/07/29 18:39:07 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_t_env(t_env *env)
{
	if (!env)
		return ;
	if (env->key)
		free(env->key);
	if (env->value)
		free(env->value);
	free(env);
}

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
			if (ft_strncmp(current->key, cmd[i], ft_strlen(cmd[i]) + 1) == 0)
			{
				del_env_variable(env, prev, current);
				break ;
			}
			prev = current;
			current = current->next;
		}
		i++;
	}

	t_env *tmp;

	tmp = *env;
	i = 0;
	while (tmp)
    {
    printf("%s\n", tmp->key);
    printf("%d\n", i);
    tmp = tmp->next;
    i++;
    }
	
	return (0);
}
