/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_status.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 03:20:11 by dgeara            #+#    #+#             */
/*   Updated: 2026/09/08 19:58:27 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief magiiic returns a pointer to the shell's persistent exit status.
 * This is used to set the exit status in child processes 
 * and retrieve it in the parent.
 */
int	*get_status(void)
{
	static int	status;

	return (&status);
}
