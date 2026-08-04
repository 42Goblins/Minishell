/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:14:00 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 18:14:01 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	ft_memset(s, 0, n);
}

/* 
int main(void)
{
	char str[] = "LIFESTYLE";

	ft_bzero(&str[3], 8);
	printf("%s\n", str);
	bzero(&str[3], 5);
	printf("%s\n", str);


	int i = 0;
	int array[] = {1,2,3,0};
	while (i < 4)
	{
		printf("%d", array[i]);
		i++;
	}
	printf("\n");
	bzero(array, sizeof(array));
	i = 0;
	while (i < 4)
	{
		printf("%d ", array[i]);
		i++;
	}
	printf("\n");

	ft_bzero(array, sizeof(array));
	i = 0;
	while (i < 4)
	{
		printf("%d ", array[i]);
		i++;
	}
	printf("\n");

	return (0);
}
 */