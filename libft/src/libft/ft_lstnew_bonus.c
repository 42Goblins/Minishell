/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 19:59:01 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/28 16:12:08 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*node;

	node = malloc(sizeof(t_list));
	if (!node)
		return (NULL);
	node->content = content;
	node->next = NULL;
	return (node);
}

/* int main(void)
{
	t_list *test1;
	int x = 42;
	test1 = ft_lstnew(&x);
	printf("%d\n", *(int *)test1->content);

	t_list *test2;
	test2 = ft_lstnew("AAA");
	printf("%s\n", (char *)test2->content);
	return (0);
} */