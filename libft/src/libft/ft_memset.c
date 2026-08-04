/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:13:11 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 18:13:12 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *b, int c, size_t len)
{
	size_t			i;
	unsigned char	*ptr;

	i = 0;
	ptr = (unsigned char *)(b);
	while (i < len)
	{
		ptr[i] = c;
		i++;
	}
	return (b);
}

/* int main(void)
{
	char str[] = "LIFESTYLE";

	ft_memset(str, '-', 8);
	printf("%s\n", str);
	memset(str, '-', 8 * sizeof(char));
	printf("%s\n", str);


	int i = 0;
	int array[] = {1,2,3,0};
	while (i < 4)
	{
		printf("%d", array[i]);
		i++;
	}
	printf("\n");
	memset(array, 254, sizeof(array));
	i = 0;
	while (i < 4)
	{
		printf("%d ", array[i]);
		i++;
	}
	printf("\n");

	ft_memset(array, 254, sizeof(array));
	i = 0;
	while (i < 4)
	{
		printf("%d ", array[i]);
		i++;
	}
	printf("\n");

	return (0);
} */
