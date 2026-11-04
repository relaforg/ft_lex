#include "cursor.h"
#include "lex_file.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
	ST_DEFINITIONS,
	ST_DECL_BLOCK,
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

	init_cursor(&cursor, buf);
	while (1)
	{
		err = get_next_cursor_line(&cursor, &line);
		if (err == -1)
			return (PARSE_ERR_ALLOC);
		if (!line)
			break ;
		printf("%s\n", line);
		free(line);
	}
	return (PARSE_OK);
}
