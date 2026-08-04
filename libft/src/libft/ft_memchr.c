/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:13:28 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/15 01:09:00 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*tmp_s;

	i = 0;
	tmp_s = (unsigned char *)s;
	while (i < n)
	{
		if (tmp_s[i] == (unsigned char)c)
			return (&tmp_s[i]);
		i++;
	}
	return (NULL);
}

/* int main(void)
{
	char s[]= "hjahk";
	char c = 'a';
	printf("%s\n", (char *)ft_memchr(s,c,10));
	printf("%s\n", (char *)memchr(s, c, 10));
	return (0);
} */