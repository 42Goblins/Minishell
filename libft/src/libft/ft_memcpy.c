/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:13:19 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/15 01:44:14 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*p_dest;
	unsigned char	*p_src;

	if (!dst && !src)
		return (NULL);
	i = 0;
	p_dest = (unsigned char *)(dst);
	p_src = (unsigned char *)(src);
	while (i < n)
	{
		p_dest[i] = p_src[i];
		i++;
	}
	return (dst);
}

/* int main(void)
{
	char s1[] = "123456789";
	char s2[] = "LIFESTYLE";
	ft_memcpy(s1, s2, 1);
	printf("%s\n", s1);

	char str1[] = "123456789";
	char str2[] = "LIFESTYLE";
	memcpy(str1, str2, 1 * sizeof(char));
	printf("%s\n", str1);

	return (0);
} */
