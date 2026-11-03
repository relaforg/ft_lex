#ifndef VEC_H
# define VEC_H

#include <stddef.h>

typedef struct {
	void *data;
	size_t len;
	size_t cap;
	size_t elem_size;
}	t_vec;

void	vec_init(t_vec *vec, size_t elem_size);
int		vec_push(t_vec *vec, const void *elem);
void	*vec_get(const t_vec *vec, size_t i);
void	vec_free(t_vec *vec, void(*free_elem)(void *));

#endif
