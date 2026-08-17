/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 01:18:00 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/12 01:18:00 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees three allocated strings.
 */
void	free_three_strings(char *first, char *second, char *third)
{
	free(first);
	free(second);
	free(third);
}

/**
 * @brief Appends an allocated part to the result and frees both strings.
 */
char	*append_expansion_part(char *built, char *part)
{
	char	*joined;

	if (!built || !part)
	{
		free(built);
		free(part);
		return (NULL);
	}
	joined = ft_strjoin(built, part);
	free(built);
	free(part);
	return (joined);
}
