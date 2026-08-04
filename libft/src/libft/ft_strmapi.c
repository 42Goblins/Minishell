/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:12:11 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 18:12:12 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	size_t	i;
	char	*new;

	i = 0;
	new = malloc(sizeof(char) * (ft_strlen(s) + 1));
	if (!new)
		return (NULL);
	while (s[i])
	{
		new[i] = f(i, s[i]);
		i++;
	}
	new[i] = '\0';
	return (new);
}
/* static char	test_tolower(unsigned int i, char c)
{
	(void)i;
	if ((c >= 'A' && c <= 'Z'))
		return (c + 32);
	return (c);
}

int main(void)
{
	char str[] = "LIFESTYLE";
	char *mapped;

	mapped = ft_strmapi(str, test_tolower);
	printf("%s\n", mapped);
	free (mapped);
	return (0);
} */