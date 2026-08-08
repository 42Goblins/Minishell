/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:13:22 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/15 01:20:57 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	size_t			i;
	unsigned char	*tmp_s1;
	unsigned char	*tmp_s2;

	i = 0;
	tmp_s1 = (unsigned char *)s1;
	tmp_s2 = (unsigned char *)s2;
	while (i < n && tmp_s1[i] == tmp_s2[i])
		i++;
	if (n == i)
		return (0);
	return (tmp_s1[i] - tmp_s2[i]);
}

/* int main(void)
{
	char s1[]= "Biugxahgfj";
	char s2[]= "Biugbaaaa";
	printf("%d\n", ft_memcmp(s1, s2, 10));
	printf("%d\n", memcmp(s1, s2, 10));

	int tab1[]= {1,2,3,4};
	int tab2[]= {1,2,3,4};
	printf("%d\n", ft_memcmp(tab1, tab2, 10));
	printf("%d\n", memcmp(tab1, tab2, 10));
	return (0);
} */
