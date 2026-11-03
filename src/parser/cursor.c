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

char	*get_next_cursor_line(t_cursor *cursor)
{
	size_t	i;
	char	*line;

	i = 0;
	if (get_cursor_char(cursor) == '\n')
		i++;
	if (!get_cursor_char(cursor))
		return (NULL);
	while (cursor->buf[cursor->pos + i] != '\n' && cursor->buf[cursor->pos + i])
		i++;
	line = strndup(&cursor->buf[cursor->pos], i);
	if (!line)
		return (NULL);
	cursor->pos += i;
	cursor->line++;
	return (line);
}
