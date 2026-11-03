#include "vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void	vec_init(t_vec *vec, size_t elem_size)
{
	vec->elem_size = elem_size;
	vec->cap = 0;
	vec->data = NULL;
	vec->len = 0;
}

int	vec_push(t_vec *vec, const void *elem)
{
	void	*tmp;
	size_t	new_cap;

	if (vec->len >= vec->cap)
	{
		new_cap = vec->cap ? vec->cap * 2 : 1;
		tmp = realloc(vec->data, new_cap * vec->elem_size);
		if (!tmp)
			return (-1);
		vec->cap = new_cap;
		vec->data = tmp;
	}
	memcpy(vec->data + (vec->len * vec->elem_size), elem, vec->elem_size);
	vec->len++;
	return (0);
}

void	*vec_get(const t_vec *vec, size_t i)
{
	if (i >= vec->len)
		return (NULL);
	return (vec->data + vec->elem_size * i);
}

void	vec_free(t_vec *vec, void (*free_elem)(void *))
{
	size_t	i;

	i = 0;
	if (free_elem)
	{
		while (i < vec->len)
		{
			free_elem(vec_get(vec, i));
			i++;
		}
	}
	free(vec->data);
}

int	main(void)
{
	t_vec	vec;

	vec_init(&vec, sizeof(int));
	for (int i = 0; i < 1000; i++)
	{
		vec_push(&vec, &i);
	}
	for (int i = 0; i < 1000; i++)
	{
		printf("%d\n", *((int *)vec_get(&vec, i)));
	}
	vec_free(&vec, NULL);
}
