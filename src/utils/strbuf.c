#include "strbuf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void	strbuf_init(t_strbuf *sb)
{
	sb->cap = 0;
	sb->len = 0;
	sb->data = NULL;
}

int	strbuf_append(t_strbuf *sb, const char *s, size_t n)
{
	size_t	new_cap;
	char	*tmp;

	if (sb->len + n >= sb->cap)
	{
		if (sb->cap + n > sb->cap * 2)
			new_cap = sb->cap + n + 1;
		else
			new_cap = sb->cap * 2 + 1;
		tmp = realloc(sb->data, new_cap * sizeof(char));
		if (!tmp)
			return (-1);
		sb->cap = new_cap;
		sb->data = tmp;
	}
	memcpy(sb->data + sb->len * sizeof(char), s, n);
	sb->len += n;
	sb->data[sb->len] = 0;
	return (0);
}

void	strbuf_free(t_strbuf *sb)
{
	sb->cap = 0;
	sb->len = 0;
	free(sb->data);
	sb->data = NULL;
}
