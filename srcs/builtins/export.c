/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 05:22:01 by dgeara            #+#    #+#             */
/*   Updated: 2026/08/09 19:47:05 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	export_error(char *str)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
	return (0);
}

void	add_new_var(t_env **env, char *key, char *value)
{
	t_env	*new;
	t_env	*tmp;

	new = malloc(sizeof(t_env));
	if (!new)
		return ;
	new->key = key;
	new->value = value;
	new->next = NULL;
	if (!*env)
	{
		*env = new;
		return ;
	}
	tmp = *env;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new;
}

int	parse_export(char *str, char **key, char **value)
{
	int	i;

	if (!str || !str[0] || !(ft_isalpha(str[0]) || str[0] == '_'))
		return (export_error(str));
	i = 0;
	while (str[i] && str[i] != '=')
	{
		if (!(ft_isalnum(str[i]) || str[i] == '_'))
			return (export_error(str));
		i++;
	}
	*key = ft_substr(str, 0, i);
	if (str[i] == '=')
		*value = ft_strdup(str + i + 1);
	else
		*value = NULL;
	if (!*key || (str[i] == '=' && !*value))
		return (export_error(str));
	return (1);
}

void	update_env_vars(t_env **env, char *key, char *value)
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
			return ;
		}
		current = current->next;
	}
	add_new_var(env, key, value);
}

int	exec_export(t_env **env, char **cmd)
{
	int		i;
	char	*cmd_key;
	char	*cmd_value;
	int		ret;

	i = 1;
	ret = 0;
	if (!cmd[1])
		return (print_export(*env), ret);
	while (cmd[i])
	{
		if (parse_export(cmd[i], &cmd_key, &cmd_value))
			update_env_vars(env, cmd_key, cmd_value);
		else
			ret = 1;
		i++;
	}
	return (ret);
}
