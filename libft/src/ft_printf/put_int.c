/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   put_int.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 08:34:22 by dgeara            #+#    #+#             */
/*   Updated: 2026/03/05 01:32:52 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	put_int(int n)
{
	int	count;

	count = 0;
	if (n == -2147483648)
		return (put_s("-2147483648"));
	else
	{
		if (n < 0)
		{
			n = n * -1;
			count += put_c('-');
		}
		if (n > 9)
			count += put_int(n / 10);
		count += put_c(n % 10 + 48);
	}
	return (count);
}
