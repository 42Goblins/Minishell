/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdelon_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 23:56:31 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/16 00:10:24 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstdelone(t_list *lst, void (*del)(void*))
{
	if (!lst || !del)
		return ;
	del(lst->content);
	free(lst);
}

/* void del(void *content)
{
    free(content);
    printf("Content freed!\n");
}

int main(void)
{
    char *str = strdup("Hello");
    t_list *node = ft_lstnew(str);
    printf("befor %s\n", (char *)node->content);
    ft_lstdelone(node, del);
	printf("after %s\n", (char *)node->content);
    return 0;
} */
