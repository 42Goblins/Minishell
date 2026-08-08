/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmauley <cmauley@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 23:25:04 by cmauley           #+#    #+#             */
/*   Updated: 2026/08/09 00:04:46 by cmauley          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

/**
 * @brief Expands variables in a word while respecting quote rules.
 */
char	*expand_word(char *word, t_env *env)
{
	int		i;
	bool	in_single;
	bool	in_double;

	(void)env;
	if (!word)
		return (NULL);
	// TODO: creer la chaine built vide et initialiser son index de depart.
	i = 0;
	in_single = false;
	in_double = false;
	while (word[i])
	{
		if (word[i] == '\'' && !in_double)
			in_single = !in_single;
		else if (word[i] == '"' && !in_single)
			in_double = !in_double;
		else if (is_dollar_expand(word, i, in_single))
		{
			// TODO: ajouter le texte situe avant le signe dollar.
			// TODO: ajouter la valeur trouvee dans l'environnement.
			// TODO: sauter le signe dollar et le nom de la variable.
			// TODO: placer start sur le prochain caractere a traiter.
		}
		i++;
	}
	// TODO: ajouter le texte restant et retourner la chaine built.
	return (ft_strdup(word));
}
