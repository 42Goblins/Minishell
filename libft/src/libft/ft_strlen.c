/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:12:15 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 18:12:16 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

/* int	main(void)
{
	const char str[] = "LIFESTYLE";
	printf("%zu\n", ft_strlen(str));
	printf("%zu\n", strlen(str));
	return (0);
} */