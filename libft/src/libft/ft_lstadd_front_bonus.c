/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 20:02:02 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/15 22:33:57 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_front(t_list **lst, t_list *new)
{
	if (!lst || !new)
		return ;
	new->next = *lst;
	*lst = new;
}

/* int main(void)
{
    t_list *test1 = ft_lstnew("1");
    t_list *test2  = ft_lstnew("2");
	t_list *test3  = ft_lstnew("3");

	ft_lstadd_front(&test3, test2);
	ft_lstadd_front(&test2, test1);
    printf("%s\n", (char *)test1->content);
    printf("%s\n", (char *)test1->next->content);
	printf("%s\n", (char *)test1->next->next->content);

    free(test2);
    free(test1->next);
    return (0);
} */
