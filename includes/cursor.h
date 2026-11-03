#ifndef CURSOR_H
# define CURSOR_H

# include <stddef.h>

typedef struct
{
	char	*buf;
	size_t	pos;
	size_t	line;
}			t_cursor;

void	init_cursor(t_cursor *cursor, char *buf);
char	get_cursor_char(t_cursor *cursor);
char	*get_next_cursor_line(t_cursor *cursor);

#endif
