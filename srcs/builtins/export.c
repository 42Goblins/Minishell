/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 05:22:01 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/30 19:13:55 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Prints an error for an invalid export identifier.
 */
int	export_error(char *str)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
	return (0);
}

/**
 * @brief Adds a variable only when its key and value are valid.
 */
int	safe_add_var(t_env **env, char *key, char *value)
{
	if (!key || !value)
		return (free(key), free(value), 0);
	return (add_new_var(env, key, value));
}

/**
 * @brief Appends a new variable to the environment list.
 */
int	add_new_var(t_env **env, char *key, char *value)
{
	t_env	*new;
	t_env	*tmp;

	new = malloc(sizeof(t_env));
	if (!new)
		return (0);
	new->key = key;
	if (value)
		new->value = value;
	else
		new->value = ft_strdup("");
	new->next = NULL;
	if (!*env)
	{
		*env = new;
		return (1);
	}
	tmp = *env;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new;
	return (1);
}

/**
 * @brief Checks if argument is a valid export argument and 
 * splits it an into its key and value.
 */
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

/**
 * @brief Handles export arguments and updates the environment.
 */
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
