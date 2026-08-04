/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 03:18:09 by dgeara            #+#    #+#             */
/*   Updated: 2026/08/04 03:18:45 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	**lst_cpy(t_env *env)
{
	t_env	**cpy;
	t_env	*tmp;
	int		lst_size;
	int		i;

	lst_size = 0;
	tmp = env;
	while (tmp)
	{
		lst_size++;
		tmp = tmp->next;
	}
	cpy = malloc(sizeof(t_env *) * (lst_size + 1));
	if (!cpy)
		return (NULL);
	i = 0;
	while (i < lst_size)
	{
		cpy[i++] = env;
		env = env->next;
	}
	cpy[lst_size] = NULL;
	return (cpy);
}

t_env	**sort_export(t_env *env)
{
	t_env	**cpy;
	t_env	*tmp;
	int		i;
	int		j;

	cpy = lst_cpy(env);
	if (!cpy)
		return (NULL);
	i = 0;
	while (cpy[i + 1])
	{
		j = 0;
		while (cpy[j + 1])
		{
			if (ft_strcmp(cpy[j]->key, cpy[j + 1]->key) > 0)
			{
				tmp = cpy[j];
				cpy[j] = cpy[j + 1];
				cpy[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
	return (cpy);
}

int	print_export(t_env *env)
{
	t_env	**env_cpy;
	int		i;

	env_cpy = sort_export(env);
	//sort_export(&env_cpy, env);
	if (!env_cpy)
		return (1);
	i = 0;
	while (env_cpy[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(env_cpy[i]->key, STDOUT_FILENO);
		if (env_cpy[i]->value)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(env_cpy[i]->value, STDOUT_FILENO);
			ft_putstr_fd("\"", STDOUT_FILENO);
		}
		ft_putstr_fd("\n", STDOUT_FILENO);
		i++;
	}
	free(env_cpy); //special free list type t_env ici plutôt ?
	return (0);
}
