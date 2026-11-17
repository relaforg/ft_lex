/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash_table.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: relaforg <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 09:42:41 by relaforg          #+#    #+#             */
/*   Updated: 2026/11/17 15:14:25 by relaforg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hash_table.h"
#include <stdint.h>
#include <stdlib.h>

size_t	hash(const uint64_t *key, size_t nwords)
{
	uint64_t		hash;
	const uint64_t	fnv_offset = 0xcbf29ce484222325;
	const uint64_t	fnv_prime = 0x100000001b3;
	size_t			w;

	hash = fnv_offset;
	w = 0;
	while (w < nwords)
	{
		hash ^= *(key + w);
		hash *= fnv_prime;
		w++;
	}
	return (hash);
}

t_htable	*init_hashtable(int table_size)
{
	t_htable	*tab;

	tab = malloc(sizeof(t_htable));
	if (tab == NULL)
		return (NULL);
	tab->size = table_size;
	tab->el_nbr = 0;
	tab->strategie = DEFAULT_MODE;
	tab->table = calloc(sizeof(t_hnode *), table_size);
	if (tab->table == NULL)
	{
		free(tab);
		return (NULL);
	}
	return (tab);
}

void	insert(t_htable **tab, const uint64_t *key, const int *value)
{
	t_hnode	*new;

	new = malloc(sizeof(t_hnode));
	if (new == NULL)
	{
		free((void *)key);
		free((void *)value);
		return ;
	}
	new->key = key;
	new->value = value;
	new->hash = hash(key, (*tab)->nwords);
	if (smart_insert(tab, new) == NULL)
	{
		(*tab)->el_nbr--;
		return ;
	}
	if (load_factor((*tab)) >= UPPER_TRESHOLD)
		increase_hashtable_size(tab);
}

t_node	*search(t_hashtable *tab, const char *key, size_t hashed_key)
{
	t_node	*curr;

	curr = tab->table[hashed_key & (tab->size - 1)];
	while (curr != NULL)
	{
		if (hashed_key == curr->hash && !ft_fstrcmp(curr->key, key))
			return (curr);
		curr = curr->next;
	}
	return (NULL);
}

void	hashtable_set_strategie(t_hashtable **tab, enum e_collide opt)
{
	(*tab)->strategie = opt;
}
