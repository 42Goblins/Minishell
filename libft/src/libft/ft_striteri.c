/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:12:33 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/14 18:12:34 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char*))
{
	int	i;

	i = 0;
	while (s[i])
	{
		f(i, &s[i]);
		i++;
	}
}
/* static void	test_tolower(unsigned int i, char *c)
{
	(void)i;
	if ((*c >= 'A' && *c <= 'Z'))
		*c += 32;
}

int main(void)
{
	char str[] = "LIFESTYLE";

	ft_striteri(str, test_tolower);
	printf("%s\n", str);
	return (0);
} */
