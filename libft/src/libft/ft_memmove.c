/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:13:15 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 18:13:16 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	size_t			i;
	unsigned char	*tmp_dst;
	unsigned char	*tmp_src;

	i = 0;
	tmp_dst = (unsigned char *)(dst);
	tmp_src = (unsigned char *)(src);
	if (tmp_dst < tmp_src)
	{
		while (i < len)
		{
			tmp_dst[i] = tmp_src[i];
			i++;
		}
	}
	else if (tmp_dst > tmp_src)
	{
		while (len > 0)
		{
			len--;
			tmp_dst[len] = tmp_src[len];
		}
	}
	return (dst);
}

/* int main(void)
{
	char s1[] = "123456789";

	ft_memmove(s1+6, s1+4, 2);
	printf("%s\n", s1);

	char str1[] = "123456789";
	memmove(str1+6, str1+4, 2 * sizeof(char));
	printf("%s\n", str1);

	return (0);
} */
