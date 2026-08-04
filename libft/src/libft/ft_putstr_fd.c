/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:12:57 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 18:12:58 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putstr_fd(char *s, int fd)
{
	while (*s)
		write(fd, s++, 1);
}
/* int	main(void)
{
	int	fd;
	char str[] = "ABC\n";

	fd = 1;
	ft_putstr_fd("AAA\n", fd);
	ft_putstr_fd(str, fd);
	printf("%c", *str);
	return (0);
} */