/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 17:01:35 by dgeara            #+#    #+#             */
/*   Updated: 2026/08/19 01:34:01 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*cpy_key(char *env)
{
	int		i;
	char	*key;

	i = 0;
	while (env[i] && env[i] != '=')
		i++;
	key = malloc(sizeof(char) * (i + 1));
	if (!key)
		return (NULL);
	ft_strlcpy(key, env, i + 1);
	return (key);
}

char	*cpy_value(char *env)
{
	int		i;
	int		j;
	char	*value;

	i = 0;
	while (env[i] && env[i] != '=')
		i++;
	if (env[i] == '=')
		i++;
	j = 0;
	while (env[i + j])
		j++;
	value = malloc(sizeof(char) * (j + 1));
	if (!value)
		return (NULL);
	ft_strlcpy(value, env + i, j + 1);
	return (value);
}

/* void setup_env(t_shell *shell, char **env)
{
	int	i;
	
	i = 0;
	while (env[i])
	{
		shell->env[i].key = cpy_key(env[i]); 
		shell->env[i].value = cpy_value(env[i]); //
		i++;
	}
} */

t_env	*new_env_node(char *env_line)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = cpy_key(env_line);
	node->value = cpy_value(env_line);
	node->next = NULL;
	if (!node->key || !node->value)
	{
		free(node->key);
		free(node->value);
		free(node);
		return (NULL);
	}
	return (node);
}

void	setup_env(t_shell *shell, char **env)
{
	int		i;
	t_env	*node;
	t_env	*last;

	shell->env = NULL;
	last = NULL;
	i = 0;
	while (env[i])
	{
		node = new_env_node(env[i]);
		if (!node)
			return ;
		if (!shell->env)
			shell->env = node;
		else
			last->next = node;
		last = node;
		i++;
	}
}
