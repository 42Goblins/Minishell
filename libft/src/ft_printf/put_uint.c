/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   put_uint.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 14:58:38 by dgeara            #+#    #+#             */
/*   Updated: 2026/03/05 01:33:03 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	put_uint(unsigned int n)
{
	int	count;

	count = 0;
	if (n > 9)
		count += put_uint(n / 10);
	count += put_c(n % 10 + 48);
	return (count);
}
