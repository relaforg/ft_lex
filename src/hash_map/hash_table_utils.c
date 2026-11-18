/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash_table_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 10:20:12 by relaforg          #+#    #+#             */
/*   Updated: 2026/11/18 08:10:57 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hash_table.h"
#include <stdlib.h>

void	free_hashtable(t_htable *tab)
{
	t_hnode	*curr;
	t_hnode	*aux;
	int		i;

	i = 0;
	while (i < tab->size)
	{
		curr = (tab->table)[i];
		while (curr != NULL)
		{
			aux = curr;
			curr = curr->next;
			free(aux);
		}
		i++;
	}
	free(tab->table);
	free(tab);
}

float	load_factor(t_htable *tab)
{
	return ((float)tab->el_nbr / (float)tab->size);
}

void	move_table(t_htable *old, t_htable *new)
{
	int		i;
	t_hnode	*curr;
	t_hnode	*aux;
	int		index;

	i = 0;
	while (i < old->size)
	{
		curr = old->table[i];
		while (curr != NULL)
		{
			index = curr->hash & (new->size - 1);
			aux = curr->next;
			curr->next = new->table[index];
			new->table[index] = curr;
			curr = aux;
		}
		i++;
	}
}

void	increase_hashtable_size(t_htable **tab)
{
	t_htable	*new;
	t_htable	*tmp;

	tmp = *tab;
	new = init_hashtable(tmp->size * 2, tmp->nwords);
	if (new == NULL)
		return ;
	move_table(tmp, new);
	new->el_nbr = tmp->el_nbr;
	*tab = new;
	free(tmp->table);
	free(tmp);
}
