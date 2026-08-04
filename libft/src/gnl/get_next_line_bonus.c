/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 13:29:56 by dgeara            #+#    #+#             */
/*   Updated: 2026/03/05 01:33:20 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	process_list(char *dst, t_list *list, int copy)
{
	int	i;
	int	j;

	j = 0;
	while (list)
	{
		i = 0;
		while (((char *)list->content)[i])
		{
			if (copy)
				dst[j] = ((char *)list->content)[i];
			j++;
			if (((char *)list->content)[i] == '\n')
			{
				if (copy)
					dst[j] = '\0';
				return (j);
			}
			i++;
		}
		list = list->next;
	}
	if (copy)
		dst[j] = '\0';
	return (j);
}

int	no_newline(t_list *lst_node)
{
	int	i;

	i = 0;
	if (!lst_node)
		return (1);
	while (((char *)lst_node->content)[i])
	{
		if (((char *)lst_node->content)[i] == '\n')
			return (0);
		i++;
	}
	return (1);
}

void	fill_list(int fd, t_list **list)
{
	char	*buffer;
	int		char_read;
	t_list	*new_node;

	buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buffer)
		return (free_list(list));
	while (no_newline(ft_lstlast(*list)))
	{
		char_read = read(fd, buffer, BUFFER_SIZE);
		if (char_read <= 0)
		{
			if (char_read < 0)
				free_list(list);
			return (free(buffer));
		}
		buffer[char_read] = '\0';
		new_node = ft_lstnew_strdup(buffer);
		if (!new_node)
			return (free(buffer), free_list(list));
		ft_lstadd_back(list, new_node);
	}
	free(buffer);
}

void	clean_list(t_list **list)
{
	t_list	*new_node;
	t_list	*last_node;
	char	*str;
	int		i;
	int		j;

	last_node = ft_lstlast(*list);
	i = process_list(NULL, last_node, 0);
	j = 0;
	str = malloc(sizeof(char) * (BUFFER_SIZE - i + 1));
	if (!str)
		return (free_list(list));
	while (((char *)last_node->content)[i])
		str[j++] = ((char *)last_node->content)[i++];
	str[j] = '\0';
	free_list(list);
	if (str[0] != '\0')
	{
		new_node = ft_lstnew_strdup(str);
		if (!new_node)
			return (free(str));
		*list = new_node;
	}
	free(str);
}

char	*get_next_line(int fd)
{
	static t_list	*list[MAX_FD];
	char			*line;

	if (fd < 0 || fd >= MAX_FD || BUFFER_SIZE < 1)
		return (NULL);
	fill_list(fd, &list[fd]);
	if (!list[fd])
		return (NULL);
	line = malloc(sizeof(char) * (process_list(NULL, list[fd], 0) + 1));
	if (!line)
		return (free_list(&list[fd]), NULL);
	process_list(line, list[fd], 1);
	if (!line[0])
		return (free(line), free_list(&list[fd]), NULL);
	clean_list(&list[fd]);
	return (line);
}
