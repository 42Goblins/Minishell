/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:03:37 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 18:11:20 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(char const *s, char c)
{
	int	i;

	i = 0;
	while (*s)
	{
		if (*s != c && (*(s + 1) == c || *(s + 1) == '\0'))
			i++;
		s++;
	}
	return (i);
}

static void	free_tab(char **tab, int i)
{
	while (i >= 0)
		free (tab[i--]);
	free (tab);
}

static int	fill_tab(char **tab, char const *s, char c)
{
	int	i;
	int	len;

	i = 0;
	while (*s)
	{
		len = 0;
		while (*s == c)
			s++;
		while (s[len] && s[len] != c)
			len++;
		if (len > 0)
		{
			tab[i] = malloc(sizeof(char) * (len + 1));
			if (!tab[i])
				return (free_tab(tab, i), 0);
			ft_strlcpy(tab[i], s, len + 1);
			s += len;
			i++;
		}
	}
	tab[i] = NULL;
	return (1);
}

char	**ft_split(char const *s, char c)
{
	char	**tab;

	if (!s)
		return (NULL);
	tab = malloc(sizeof(char *) * (count_words(s, c) + 1));
	if (!tab)
		return (NULL);
	if (fill_tab(tab, s, c) == 0)
		return (NULL);
	return (tab);
}

/* int main(void)
{
	int i = 0;
	char *s1 = "---DO YOU KNO---42-";
	char **split = ft_split(s1, '-');

	while (split[i])
		printf("[%s]\n", split[i++]);
	return (0);
} */