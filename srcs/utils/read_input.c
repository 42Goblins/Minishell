/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_input.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 00:00:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/09/28 00:00:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Appends one character to a line read without readline.
 */
static char	*append_input_char(char *line, char c, size_t len)
{
	char	*new_line;
	size_t	i;

	new_line = malloc(sizeof(char) * (len + 2));
	if (!new_line)
		return (free(line), NULL);
	i = 0;
	while (i < len)
	{
		new_line[i] = line[i];
		i++;
	}
	new_line[i] = c;
	new_line[i + 1] = '\0';
	free(line);
	return (new_line);
}

/**
 * @brief Reads one line from stdin without readline buffering.
 */
static char	*read_without_prompt(void)
{
	char	*line;
	char	c;
	ssize_t	bytes;
	size_t	len;

	line = NULL;
	len = 0;
	bytes = read(STDIN_FILENO, &c, 1);
	while (bytes > 0)
	{
		if (c == '\n')
			break ;
		line = append_input_char(line, c, len);
		if (!line)
			return (NULL);
		len++;
		bytes = read(STDIN_FILENO, &c, 1);
	}
	if (bytes <= 0 && len == 0)
		return (NULL);
	return (line);
}

/**
 * @brief Reads one input line with a prompt only in interactive mode.
 */
char	*read_input(char *prompt)
{
	if (isatty(STDIN_FILENO))
		return (readline(prompt));
	return (read_without_prompt());
}
