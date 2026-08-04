/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   put_x.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 14:58:38 by dgeara            #+#    #+#             */
/*   Updated: 2026/03/05 01:33:06 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	put_x(unsigned int n, char *base)
{
	int		count;
	size_t	base_len;

	count = 0;
	base_len = ft_strlen(base);
	if (n >= base_len)
		count += put_x(n / base_len, base);
	count += put_c(base[n % base_len]);
	return (count);
}
