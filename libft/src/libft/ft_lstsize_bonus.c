/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 22:35:52 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/15 22:57:43 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	int	i;

	i = 0;
	while (lst)
	{
		lst = lst->next;
		i++;
	}
	return (i);
}

/* int main(void)
{
    t_list *test1 = ft_lstnew("1");
    t_list *test2  = ft_lstnew("2");
	t_list *test3  = ft_lstnew("3");

	ft_lstadd_front(&test3, test2);
	ft_lstadd_front(&test2, test1);
    printf("%d\n", ft_lstsize(test1));

    t_list *tmp;
    while (test1)
    {
        tmp = test1->next;
        free(test1);
        test1 = tmp;
    }
    return (0);
} */