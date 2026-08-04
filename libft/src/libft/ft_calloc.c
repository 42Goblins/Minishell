/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:13:57 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 18:13:58 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	unsigned char	*ret;

	ret = malloc(count * size);
	if (!ret)
		return (NULL);
	return (ft_memset(ret, 0, count * size));
}

/* int main(void)
{
	printf("%s\n", (char *)ft_calloc(10, 1));
	printf("%s\n", (char *)calloc(10 , 1));
	return (0);
} */