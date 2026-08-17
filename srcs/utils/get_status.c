/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_status.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 00:43:42 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/12 00:44:23 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Returns the shared storage for the last command status.
 */
int	*get_status(void)
{
	static int	status;

	return (&status);
}
