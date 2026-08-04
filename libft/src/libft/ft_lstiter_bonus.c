/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstiter_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 00:26:04 by dgeara            #+#    #+#             */
/*   Updated: 2025/10/16 02:16:17 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstiter(t_list *lst, void (*f)(void *))
{
	while (lst && f)
	{
		f(lst->content);
		lst = lst->next;
	}
}

/* static void to_lower2(void *content)
{
    char *str = (char *)content;
    int i = 0;
    while (str[i])
    {
        str[i] = ft_tolower(str[i]);
        i++;
    }
}

int main(void)
{
    t_list *test1 = ft_lstnew(strdup("A"));
    t_list *test2 = ft_lstnew(strdup("B"));
    t_list *test3 = ft_lstnew(strdup("C"));

    ft_lstadd_back(&test1, test2);
    ft_lstadd_back(&test1, test3);

    ft_lstiter(test1, to_lower2);

    printf("%s\n", (char *)test1->content);
    printf("%s\n", (char *)test1->next->content);
    printf("%s\n", (char *)test1->next->next->content);

    ft_lstclear(&test1, free);
    return 0;
} */
