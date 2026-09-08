/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 18:38:33 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/08 20:39:37 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * This file converts a validated token list into t_cmd nodes.
 * Redirections are skipped from cmd_and_args and opened separately.
 */

static int	copy_word_to_args(char **cmd_and_args, int *i, char *value);

/**
 * @brief Converts prepared tokens into a linked command list split by pipes.
 */
t_cmd	*parse_tokens(t_token *tokens)
{
	t_token	*current;
	t_cmd	*cmds;
	t_cmd	*new_cmd;
	t_cmd	*last_cmd;

	current = tokens;
	cmds = NULL;
	new_cmd = NULL;
	last_cmd = NULL;
	while (current)
	{
		new_cmd = create_cmd_node(current);
		if (!new_cmd)
			return (free_cmds(cmds), NULL);
		if (cmds == NULL)
			cmds = new_cmd;
		else
			last_cmd->next = new_cmd;
		last_cmd = new_cmd;
		while (current && current->type != T_PIPE)
			current = current->next;
		if (current)
			current = current->next;
	}
	return (cmds);
}

/**
 * @brief Counts command arguments before the next pipe.
 *
 * Redirection operators and their target word are skipped because they must not
 * appear in cmd_and_args.
 */
int	count_cmd_args(t_token *tokens)
{
	t_token	*current;
	int		count;

	count = 0;
	current = tokens;
	while (current)
	{
		if (current->type == T_PIPE)
			return (count);
		if (is_redirection_token(current->type))
		{
			if (current->next)
				current = current->next;
		}
		else if (current->type == T_WORD)
			count++;
		current = current->next;
	}
	return (count);
}

/**
 * @brief Creates the cmd_and_args array before the next pipe.
 *
 * Redirection operators and their target word are skipped. The returned array
 * is NULL-terminated and must be freed by the caller.
 */
char	**create_cmd_and_args(t_token *tokens)
{
	t_token	*current;
	int		i;
	char	**cmd_and_args;

	cmd_and_args = malloc(sizeof(char *) * (count_cmd_args(tokens) + 1));
	if (!cmd_and_args)
		return (NULL);
	current = tokens;
	i = 0;
	while (current && current->type != T_PIPE)
	{
		if (is_redirection_token(current->type))
		{
			if (current->next)
				current = current->next;
		}
		else if (current->type == T_WORD)
		{
			if (copy_word_to_args(cmd_and_args, &i, current->value))
				return (NULL);
		}
		current = current->next;
	}
	cmd_and_args[i] = NULL;
	return (cmd_and_args);
}

/**
 * @brief Copies one word token value into cmd_and_args and advances the index.
 */
static int	copy_word_to_args(char **cmd_and_args, int *i, char *value)
{
	cmd_and_args[*i] = ft_strdup(value);
	if (cmd_and_args[*i] == NULL)
	{
		cmd_and_args[*i] = NULL;
		free_tab(cmd_and_args);
		return (1);
	}
	(*i)++;
	return (0);
}

/**
 * @brief Allocates one command node with args, redirections and builtin flag.
 */
t_cmd	*create_cmd_node(t_token *tokens)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->cmd_and_args = NULL;
	cmd->path = NULL;
	cmd->fd_in = 0;
	cmd->fd_out = 1;
	cmd->access_check = false;
	cmd->next = NULL;
	cmd->cmd_and_args = create_cmd_and_args(tokens);
	if (!cmd->cmd_and_args)
		return (free(cmd), NULL);
	cmd->is_builtin = check_is_builtins(cmd->cmd_and_args[0]);
	if (open_redirections(cmd, tokens))
		return (free_cmds(cmd), NULL);
	return (cmd);
}
