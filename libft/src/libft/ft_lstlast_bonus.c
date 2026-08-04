/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 23:02:05 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/15 23:18:09 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstlast(t_list *lst)
{
	while (lst && lst->next)
		lst = lst->next;
	return (lst);
}

/* int main(void)
{
	t_list *test1 = ft_lstnew("1");
	t_list *test2  = ft_lstnew("2");
	t_list *test3  = ft_lstnew("3");

	ft_lstadd_front(&test3, test2);
	ft_lstadd_front(&test2, test1);
	printf("%s\n", (char *)ft_lstlast(test1)->content);

	t_list *tmp;
	while (test1)
	{
		tmp = test1->next;
		free(test1);
		test1 = tmp;
	}
	return (0); 
} */