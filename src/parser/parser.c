#include "cursor.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
	DEFINITIONS,
	RULES,
	USER_CODE
}	State;

int	parse(char *buf)
{
	t_cursor	cursor;
	char		*line;

	// State		state;
	if (!buf)
		return (-1);
	// state = DEFINITIONS;
	init_cursor(&cursor, buf);
	while (1)
	{
		line = get_next_cursor_line(&cursor);
		if (!line)
			break ;
		printf("%s", line);
		free(line);
	}
	return (0);
}
