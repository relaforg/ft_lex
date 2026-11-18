#ifndef HASH_TABLE_H
# define HASH_TABLE_H

# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>

# define UPPER_TRESHOLD 0.8
# define INIT_SIZE		64

typedef struct s_hnode
{
	const uint64_t		*key;
	int					value;
	size_t				hash;
	struct s_hnode		*next;
}	t_hnode;

typedef struct s_hashtable
{
	t_hnode			**table;
	int				size;
	int				el_nbr;
	size_t			nwords;
}	t_htable;

size_t		hash(const uint64_t *key, size_t nwords);
void		init_hashtable(t_htable *tab, size_t nwords);
void		free_hashtable(t_htable *tab);
int			insert(t_htable *tab, const uint64_t *key, size_t hash_key, int value);
t_hnode		*search(const t_htable *tab, const uint64_t *key, size_t hashed_key);
float		load_factor(t_htable *tab);
int			increase_hashtable_size(t_htable *tab);

#endif
