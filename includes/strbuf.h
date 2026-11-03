#ifndef STRBUF_H
# define STRBUF_H

#include <stddef.h>

typedef struct {
	char *data;
	size_t len;
	size_t cap;
}	t_strbuf;

void	strbuf_init(t_strbuf *sb);
int		strbuf_append(t_strbuf *sb, const char *s, size_t n);
void	strbuf_free(t_strbuf *sb);

#endif
