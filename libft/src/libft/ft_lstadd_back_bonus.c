/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 23:20:17 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/27 08:51:28 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*last;

	last = ft_lstlast(*lst);
	if (last)
		last->next = new;
	else
		*lst = new;
}

/* int main(void)
{
	t_list *test1 = ft_lstnew("1");
	t_list *test2  = ft_lstnew("2");
	t_list *test3  = ft_lstnew("3");

	ft_lstadd_back(&test2, test3);
	ft_lstadd_back(&test1, test2);
	printf("%s\n", (char *)test1->content);
    printf("%s\n", (char *)test1->next->content);
	printf("%s\n", (char *)test1->next->next->content);

	t_list *tmp;
	while (test1)
	{
		tmp = test1->next;
		free(test1);
		test1 = tmp;
	}
	return (0); 
} */