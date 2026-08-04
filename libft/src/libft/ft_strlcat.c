/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:12:26 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 18:12:27 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	len_d;
	size_t	i;

	len_d = 0;
	i = 0;
	while (len_d < dstsize && dst[len_d])
		len_d++;
	if (len_d == dstsize)
		return (len_d + ft_strlen(src));
	while ((len_d + i) < dstsize - 1 && src[i])
	{
		dst[len_d + i] = src[i];
		i++;
	}
	dst[len_d + i] = '\0';
	return (len_d + ft_strlen(src));
}

/* int main(void)
{
	char s1[30] = "LIFESTYLE";
	char s2[] = "123456789";
	ft_strlcat(s1, s2, 20);
	printf("%s\n", s1);

	char str1[30] = "LIFESTYLE";
	char str2[] = "123456789";
	strlcat(str1, str2, 20);
	printf("%s\n", str1);

	return (0);
} */