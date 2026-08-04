/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 11:42:45 by dgeara            #+#    #+#             */
/*   Updated: 2026/03/05 01:31:56 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	call_f(char c, va_list args, int *len)
{
	if (c == '%')
		*len += (put_c('%'));
	else if (c == 'i' || c == 'd')
		*len += put_int(va_arg(args, int));
	else if (c == 'u')
		*len += put_uint(va_arg(args, unsigned int));
	else if (c == 'x')
		*len += put_x(va_arg(args, unsigned int), "0123456789abcdef");
	else if (c == 'X')
		*len += put_x(va_arg(args, unsigned int), "0123456789ABCDEF");
	else if (c == 's')
		*len += put_s(va_arg(args, char *));
	else if (c == 'c')
		*len += put_c(va_arg(args, int));
	else if (c == 'p')
		*len += put_p(va_arg(args, void *));
}

int	ft_printf(const char *str, ...)
{
	int		len;
	va_list	args;

	len = 0;
	va_start(args, str);
	while (*str)
	{
		if (*str == '%')
		{
			call_f(*(str + 1), args, &len);
			str++;
		}
		else
			len += put_c(*str);
		str++;
	}
	va_end(args);
	return (len);
}
