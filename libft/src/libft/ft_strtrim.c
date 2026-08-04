/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:11:54 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 18:11:55 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	in_set(char c, char const *set)
{
	while (*set)
	{
		if (*set == c)
			return (1);
		set++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*str;
	size_t	start;
	size_t	end;

	if (!s1 || !set)
		return (NULL);
	start = 0;
	end = ft_strlen(s1);
	while (s1[start] && in_set(s1[start], set))
		start++;
	while (end > start && in_set(s1[end - 1], set))
		end--;
	str = malloc(sizeof(char) * ((end - start) + 1));
	if (!str)
		return (NULL);
	ft_strlcpy(str, &s1[start], (end - start) + 1);
	return (str);
}

/* int main(void)
{
	char *s1 = "LIFE123STYLE32";
	char *s2 = "123";

	printf("%s\n", ft_strtrim(s1,s2));
	return (0);
} */