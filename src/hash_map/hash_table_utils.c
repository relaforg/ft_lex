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
	tab->table = NULL;
	tab->el_nbr = 0;
	tab->size = 0;
}

float	load_factor(t_htable *tab)
{
	return ((float)tab->el_nbr / (float)tab->size);
}

static void	move_table(const t_htable *tab, t_hnode **new, int new_size)
{
	int		i;
	t_hnode	*curr;
	t_hnode	*next;
	size_t	index;

	i = 0;
	while (i < tab->size)
	{
		curr = tab->table[i];
		while (curr != NULL)
		{
			next = curr->next;
			index = curr->hash & (size_t)(new_size - 1);
			curr->next = new[index];
			new[index] = curr;
			curr = next;
		}
		i++;
	}
}

int	increase_hashtable_size(t_htable *tab)
{
	t_hnode	**new;
	size_t	new_size;

	new_size = tab->size ? tab->size * 2 : INIT_SIZE;
	new = calloc(sizeof(t_hnode *), new_size);
	if (new == NULL)
		return (-1);
	move_table(tab, new, new_size);
	free(tab->table);
	tab->size = new_size;
	tab->table = new;
	return (0);
}
