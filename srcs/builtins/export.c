/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 05:22:01 by dgeara            #+#    #+#             */
/*   Updated: 2026/08/03 16:25:40 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int parse_export(char *str)
{
    int i;

    if (!str || !str[0] || !(ft_isalpha(str[0]) || str[0] == '_'))
    {
        ft_putstr_fd("minishell: export: `", STDERR_FILENO);
        ft_putstr_fd(str, STDERR_FILENO);
        ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
        return (0);
    }
    i = 1;
    while (str[i] && str[i] != '=')
    {
        if (!(ft_isalnum(str[i]) || str[i] == '_'))
        {
            ft_putstr_fd("minishell: export: `", STDERR_FILENO);
            ft_putstr_fd(str, STDERR_FILENO);
            ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
            return (0);
        }
        i++;
    }
    return (1);
}

int lst_cpy(t_env ***cpy, t_env *env)
{
    int lst_size;
    int i;

    lst_size = ft_lstsize(env);
    *cpy = malloc(sizeof(t_env *) * (lst_size + 1));
    if (!*cpy)
        return (1);
    i = 0;
    while (i < lst_size)
    {
        (*cpy)[i++] = env;
        env = env->next;
    }
    (*cpy)[lst_size] = NULL;
    return (0);
}

void **lst_cpy(t_env **cpy, t_env *env)
{
    int     lst_size;
    int     i;

    lst_size = ft_lstsize(env);
    cpy = malloc(sizeof(t_env *) * (lst_size + 1));
    if (!cpy)
        return;
    i = 0;
    while (i < lst_size)
    {
        cpy[i++] = env;
        env = env->next;
    }
    cpy[lst_size] = NULL;
}

void **sort_export(t_env **cpy, t_env *env)
{
    t_env   *tmp;
    int     i;
    int     j;

    lst_cpy(cpy, env);
    if (!cpy)
        return;
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
}

int print_export(t_env *env)
{
    t_env   **env_cpy;
    int     i;

    sort_export(&env_cpy, env);
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
// print export version nn classé ordre alphabethique
/* int	print_export(t_env *env)
{
	// cpy env
	// tri with swap
	// print
	// free
	while (env)
	{
		if(env->key)
		{
			ft_putstr_fd("declare -x ", STDOUT_FILENO);
			ft_putstr_fd(env->key, STDOUT_FILENO);
			if (env->value)
			{
				ft_putstr_fd("=\"", STDOUT_FILENO);
				ft_putstr_fd(env->value, STDOUT_FILENO);
				ft_putstr_fd("\"", STDOUT_FILENO);
			}
		}
		ft_putstr_fd("\n", STDOUT_FILENO);
		env = env->next;
	}
	return (0);
} */

int	exec_export(t_env **env, char **cmd)
{
	int		i;
	t_env	*current;
	t_env	*prev;

	i = 1;
	if(!cmd[i])
		print_env_export(&env);	
	while (cmd[i])
	{
		if (!parse_export(cmd[i]))
		{
			// update ou add var
			current = *env;
			prev = NULL;
			while (current)
			{
				if (ft_strncmp(current->key, cmd[i], ft_strlen(cmd[i]) + 1) == 0)
				{
					modifie_env_var_value(cmd[i]);
				}
				prev = current;
				current = current->next;
			}
		}
		// verif le return si flop parse
/* 		else
			sig = 1 */
		i++;
	}
	return (0);
}
