/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   put_p.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 14:58:38 by dgeara            #+#    #+#             */
/*   Updated: 2026/03/05 01:32:57 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	put_ulong_base(unsigned long n, char *base)
{
	int		count;
	size_t	base_len;

	count = 0;
	base_len = ft_strlen(base);
	if (n >= base_len)
		count += put_ulong_base(n / base_len, base);
	count += put_c(base[n % base_len]);
	return (count);
}

int	put_p(void *a)
{
	int				count;
	unsigned long	adresse;

	count = 0;
	adresse = (unsigned long)a;
	count += put_s("0x");
	count += put_ulong_base(adresse, "0123456789abcdef");
	return (count);
}
