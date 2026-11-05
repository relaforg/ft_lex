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
int		get_next_cursor_line(t_cursor *cursor, char **buf);
void	cursor_advance(t_cursor *cursor);
void	cursor_skip_blanks(t_cursor *cursor);
void	cursor_skip_line(t_cursor *cursor);
int		cursor_starts_with(t_cursor *cursor, const char *s);

#endif
