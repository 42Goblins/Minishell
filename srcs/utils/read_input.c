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

/*
 * Interactive input: use readline with a visible prompt.
 * Piped/file input: read silently, like bash.
 * read() is used for silent input because it does not keep hidden buffered
 * data that could be duplicated by the heredoc fork.
 */

/**
 * @brief Creates a new line containing the old line plus one character.
 */
static char	*add_char_to_line(char *line, char c, size_t len)
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
 * @brief Reads one stdin line without readline or hidden buffering.
 */
static char	*read_line_without_readline(void)
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
		line = add_char_to_line(line, c, len);
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
 * @brief Reads one input line, using a prompt only in interactive mode.
 */
char	*read_input(char *prompt)
{
	if (isatty(STDIN_FILENO))
		return (readline(prompt));
	return (read_line_without_readline());
}
