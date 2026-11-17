#ifndef HASH_TABLE_H
# define HASH_TABLE_H

# include <stdbool.h>
# include <stddef.h>
#include <stdint.h>

# ifndef KEEP_READING
#  define KEEP_READING 0
# endif

# ifndef DEFAULT_MODE
#  define DEFAULT_MODE 1
# endif

# define UPPER_TRESHOLD 0.8

typedef struct s_hnode
{
	const uint64_t		*key;
	const int			*value;
	size_t				hash;
	struct s_hnode		*next;
}	t_hnode;

enum	e_collide
{
	DEFAULT,
	REPLACE,
	CONCAT,
};

typedef struct s_hashtable
{
	enum e_collide	strategie;
	t_hnode			**table;
	int				size;
	int				el_nbr;
	size_t			nwords;
}	t_htable;

size_t		hash(const uint64_t *key, size_t nwords);
t_htable	*init_hashtable(int table_size);
void		free_hashtable(t_htable *tab, bool delete);
void		show_hashtable(t_htable *tab);
void	insert(t_htable **tab, const uint64_t *key, const int *value);
t_hnode		*search(t_htable *tab, const char *key, size_t hashed_key);
void		delete_from_hashtable(t_htable **tab, const char *key);
float		load_factor(t_htable *tab);
void		increase_hashtable_size(t_htable **tab);
void		hashtable_set_strategie(t_htable **tab, enum e_collide opt);
void		*smart_insert(t_htable **tab, t_hnode *new);

#endif
