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
		hash ^= hash >> 32;
		w++;
	}
	return (hash);
}

void	init_hashtable(t_htable *tab, size_t nwords)
{
	tab->size = 0;
	tab->el_nbr = 0;
	tab->nwords = nwords;
	tab->table = NULL;
}

int	insert(t_htable *tab, const uint64_t *key, size_t hash_key, int value)
{
	t_hnode	*new;

	if ((tab->size == 0 || load_factor(tab) >= UPPER_TRESHOLD)
		&& increase_hashtable_size(tab) == -1)
		return (-1);
	new = malloc(sizeof(t_hnode));
	if (new == NULL)
		return (-1);
	new->key = key;
	new->value = value;
	new->hash = hash_key;
	new->next = tab->table[hash_key & (tab->size - 1)];
	tab->table[hash_key & (tab->size - 1)] = new;
	tab->el_nbr++;
	return (0);
}

t_hnode	*search(const t_htable *tab, const uint64_t *key, size_t hashed_key)
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
