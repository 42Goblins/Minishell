/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 17:33:49 by dgeara            #+#    #+#             */
/*   Updated: 2026/06/17 20:24:11 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize)
{
	size_t	i;

	i = 0;
	if (dstsize == 0)
		return (ft_strlen(src));
	while (i < dstsize - 1 && src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (ft_strlen(src));
}

/* int main(void)
{
	char s1[] = "LIFESTYLE";
	char s2[] = "123456789jkh";
	ft_strlcpy(s1, s2, ft_strlen(s1)+2);
	printf("%s\n", s1);

	char str1[] = "LIFESTYLE";
	char str2[] = "123456789";
	strlcpy(str1, str2, ft_strlen(str1)+2);
	printf("%s\n", str1);

	return (0);
} */
