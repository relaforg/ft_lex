#include "cursor.h"
#include <string.h>

void	init_cursor(t_cursor *cursor, char *buf)
{
	cursor->buf = buf;
	cursor->pos = 0;
	cursor->line = 0;
}

char	get_cursor_char(t_cursor *cursor)
{
	return (cursor->buf[cursor->pos]);
}

int	get_next_cursor_line(t_cursor *cursor, char **buf)
{
	size_t	i;

	if (!get_cursor_char(cursor))
	{
		*buf = 0;
		return (0);
	}
	i = 0;
	while (cursor->buf[cursor->pos + i] != '\n' && cursor->buf[cursor->pos + i])
		i++;
	*buf = strndup(&cursor->buf[cursor->pos], i);
	if (!*buf)
		return (-1);
	cursor->pos += i;
	cursor->line++;
	if (get_cursor_char(cursor) == '\n')
		cursor->pos++;
	return (0);
}

void	cursor_advance(t_cursor *cursor)
{
	if (!get_cursor_char(cursor))
		return ;
	if (get_cursor_char(cursor) == '\n')
		cursor->line++;
	cursor->pos++;
}

void	cursor_skip_blanks(t_cursor *cursor)
{
	while ((get_cursor_char(cursor) == ' ' || get_cursor_char(cursor) == '\t'))
		cursor_advance(cursor);
}

void	cursor_skip_line(t_cursor *cursor)
{
	while (get_cursor_char(cursor) && get_cursor_char(cursor) != '\n')
		cursor_advance(cursor);
	cursor_advance(cursor);
}

int	cursor_starts_with(t_cursor *cursor, const char *s)
{
	return (strncmp(cursor->buf + cursor->pos, s, strlen(s)) == 0);
}
