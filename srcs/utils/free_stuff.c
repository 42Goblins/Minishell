/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_stuff.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 03:21:34 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/29 04:19:23 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees one environment node and its allocated key/value strings.
 */
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

/**
 * @brief Frees the full environment linked list.
 */
void	free_lst_env(t_env *env)
{
	t_env	*next;

	while (env)
	{
		next = env->next;
		free_t_env(env);
		env = next;
	}
}

/**
 * @brief Frees a NULL-terminated array of allocated strings.
 */
void	free_tab(char **tab)
{
	int	i;

	if (!tab)
		return ;
	i = 0;
	while (tab[i])
		free(tab[i++]);
	free(tab);
}
