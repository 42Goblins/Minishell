/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:11:42 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/15 14:59:27 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_tolower(int c)
{
	if ((c >= 'A' && c <= 'Z'))
		return (c + 32);
	return (c);
}

/* int main(void)
{
	char c;
	c = 'A';
	printf("%c\n", ft_tolower(c));
	printf("%c\n", tolower(c));
	return (0);
} */
