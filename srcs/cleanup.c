/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgeara <dgeara@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 03:41:13 by dgeara            #+#    #+#             */
/*   Updated: 2026/07/20 21:31:18 by dgeara           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief 
 * ici add tout cleanup pas oublié les check si les éléments exists 
 * avant d'essayer de free
 */

void cleanup(t_shell *shell)
{
	rl_clear_history();
}