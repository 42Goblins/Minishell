/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:12:07 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/15 00:49:23 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while ((i < n && s1[i] == s2[i]) && s1[i])
		i++;
	if (i == n)
		return (0);
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

/* int main(void)
{
	char s1[]= "David";
	char s2[]= "Davide";
	printf("%d\n", ft_strncmp(s1, s2, 5));
	printf("%d\n", strncmp(s1, s2, 5));
	return (0);
} */
