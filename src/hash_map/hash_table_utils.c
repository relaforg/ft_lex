/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash_table_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 10:20:12 by relaforg          #+#    #+#             */
/*   Updated: 2026/03/01 18:21:15 by secros           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hash_table.h"
#include <stdlib.h>

void	free_hashtable(t_hashtable *tab, bool delete)
{
	t_node	*curr;
	t_node	*aux;
	int		i;

	i = 0;
	while (i < tab->size)
	{
		curr = (tab->table)[i];
		while (curr != NULL)
		{
			aux = curr;
			curr = curr->next;
			if (delete)
			{
				free((void *)aux->key);
				free((void *)aux->value);
			}
			free(aux);
		}
		i++;
	}
	free(tab->table);
	free(tab);
}

float	load_factor(t_hashtable *tab)
{
	return ((float)tab->el_nbr / (float)tab->size);
}

void	move_table(t_hashtable *old, t_hashtable *new)
{
	int			i;
	t_node		*curr;
	t_node		*aux;
	int			index;

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

void	increase_hashtable_size(t_hashtable **tab)
{
	t_hashtable	*new;
	t_hashtable	*tmp;

	tmp = *tab;
	new = init_hashtable(tmp->size * 2);
	if (new == NULL)
		return ;
	new->strategie = tmp->strategie;
	move_table(tmp, new);
	new->el_nbr = tmp->el_nbr;
	*tab = new;
	free(tmp->table);
	free(tmp);
}
