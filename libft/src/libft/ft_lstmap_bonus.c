/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 02:16:44 by dgeara            #+#    #+#             */
/*   Updated: 2026/02/27 02:48:12 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*new_head;
	t_list	*new_node;
	void	*new_content;

	if (!lst || !f || !del)
		return (NULL);
	new_head = NULL;
	while (lst)
	{
		new_content = f(lst->content);
		new_node = ft_lstnew(new_content);
		if (!new_node)
		{
			del(new_content);
			ft_lstclear(&new_head, del);
			return (NULL);
		}
		ft_lstadd_back(&new_head, new_node);
		lst = lst->next;
	}
	return (new_head);
}

/* static void del(void *content)
{
	free(content);
}

static void *to_lower2(void *content)
{
	char *str = (char *)content;
	int i = 0;
	while (str[i])
	{
		str[i] = ft_tolower(str[i]);
		i++;
	}
	return (str);
}


int main(void)
{
	t_list *test1 = ft_lstnew(strdup("A"));
	t_list *test2 = ft_lstnew(strdup("B"));
	t_list *test3 = ft_lstnew(strdup("C"));

	ft_lstadd_back(&test1, test2);
	ft_lstadd_back(&test1, test3);

	ft_lstmap(test1, to_lower2, del);

	printf("%s\n", (char *)test1->content);
	printf("%s\n", (char *)test1->next->content);
	printf("%s\n", (char *)test1->next->next->content);

	ft_lstclear(&test1, free);
	return 0;
} */