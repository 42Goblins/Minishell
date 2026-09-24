/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 03:33:43 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/18 05:13:48 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Updates the VALUE of an existing env node, freeing its
 * previous value.
 *
 * Does nothing if key is not found in the env list.
 */
void	set_env_value(t_env *env, char *key, char *value)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
		{
			free(env->value);
			env->value = ft_strdup(value);
			return ;
		}
		env = env->next;
	}
}

/**
 * @brief Returns the value of the corresponding key in the env list, or
 * NULL if key is not found.
 */
char	*get_env_value(t_env *env, char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/**
 * @brief Updates env node VALUE corresponding to key or creates 
 * a new node if key is not found.
 *
 */
int	update_env_vars(t_env **env, char *key, char *value)
{
	t_env	*current;

	current = *env;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			free(key);
			if (value)
			{
				free(current->value);
				current->value = value;
			}
			return (0);
		}
		current = current->next;
	}
	return (add_new_var(env, key, value));
}
