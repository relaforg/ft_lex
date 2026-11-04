#include "cursor.h"
#include "lex_file.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
	ST_DECL_BLOCK = 0,
	ST_DEFINITIONS,
	ST_RULES,
	ST_USER_CODE
}				t_state;

typedef struct
{
	t_cursor	cursor;
	t_state		state;
	size_t		block_line;
	t_lex_file	*out;
}				t_parser;

t_parse_error	parse(char *buf)
{
	char		*line;
	t_cursor	cursor;
	int			err;
	t_state		state;

	state = ST_DEFINITIONS;
	init_cursor(&cursor, buf);
	while (1)
	{
		err = get_next_cursor_line(&cursor, &line);
		if (err == -1)
			return (PARSE_ERR_ALLOC);
		if (!line)
			break ;
		if (!strncmp(line, "%%", 2))
		{
			state++;
			free(line);
			continue ;
		}
		free(line);
	}
	if (state <= ST_DEFINITIONS)
		return (PARSE_ERR_NO_SEPARATOR);
	return (PARSE_OK);
}
