/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 17:34:22 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 17:34:25 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_toupper(int c)
{
	if ((c >= 'a' && c <= 'z'))
		return (c - 32);
	return (c);
}

/* int main(void)
{
	char c;
	c = 'P';
	printf("%c\n", ft_toupper(c));
	printf("%c\n", toupper(c));
	return (0);
} */
