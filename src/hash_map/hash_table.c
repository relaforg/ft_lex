#include "bitset.h"
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

t_htable	*init_hashtable(int table_size, size_t nwords)
{
	t_htable	*tab;

	tab = malloc(sizeof(t_htable));
	if (tab == NULL)
		return (NULL);
	tab->size = table_size;
	tab->el_nbr = 0;
	tab->nwords = nwords;
	tab->table = calloc(sizeof(t_hnode *), table_size);
	if (tab->table == NULL)
	{
		free(tab);
		return (NULL);
	}
	return (tab);
}

int	insert(t_htable **tab, const uint64_t *key, size_t hash_key, int value)
{
	t_hnode		*new;
	t_htable	*tmp;

	tmp = *tab;
	new = malloc(sizeof(t_hnode));
	if (new == NULL)
		return (-1);
	new->key = key;
	new->value = value;
	new->hash = hash_key;
	new->next = tmp->table[hash_key & (tmp->size - 1)];
	tmp->table[hash_key & (tmp->size - 1)] = new;
	tmp->el_nbr++;
	if (load_factor((*tab)) >= UPPER_TRESHOLD)
		increase_hashtable_size(tab);
	return (0);
}

t_hnode	*search(t_htable *tab, const uint64_t *key, size_t hashed_key)
{
	t_hnode	*curr;

	curr = tab->table[hashed_key & (tab->size - 1)];
	while (curr != NULL)
	{
		if (hashed_key == curr->hash && bitset_eq(curr->key, key, tab->nwords))
			return (curr);
		curr = curr->next;
	}
	return (NULL);
}
