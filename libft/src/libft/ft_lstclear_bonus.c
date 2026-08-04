/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 00:37:17 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/16 00:37:34 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void*))
{
	t_list	*tmp;

	if (!lst || !del)
		return ;
	while (*lst)
	{
		tmp = (*lst)->next;
		ft_lstdelone(*lst, del);
		*lst = tmp;
	}
}

/* void del(void *content)
{
    free(content);
    printf("Content freed!\n");
}

int main(void)
{
    t_list *node1 = ft_lstnew(strdup("1"));
    t_list *node2 = ft_lstnew(strdup("2"));
    t_list *node3 = ft_lstnew(strdup("3"));

    ft_lstadd_back(&node1, node2);
    ft_lstadd_back(&node1, node3);

    ft_lstclear(&node1, del);

    printf("%s\n", node1 == NULL ? "List cleared" : "Not cleared");

    return 0;
} */