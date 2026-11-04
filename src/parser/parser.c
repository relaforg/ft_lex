#include "cursor.h"
#include "lex_file.h"
#include "parse.h"
#include "strbuf.h"
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

t_parse_error	extract_ere(char *line, size_t *end)
{
	char	in_quotes;
	char	in_brackets;

	in_quotes = 0;
	in_brackets = 0;
	*end = 0;
	while (line[*end])
	{
		if (line[*end] == '\\' && line[*end + 1])
			(*end)++;
		else if (in_quotes)
			in_quotes = (line[*end] != '"');
		else if (in_brackets)
			in_brackets = (line[*end] != ']');
		else if (line[*end] == '"')
			in_quotes = 1;
		else if (line[*end] == '[')
		{
			in_brackets = 1;
			if (line[*end + 1] && line[*end + 1] == '^' && line[*end + 2]
				&& line[*end + 2] == ']')
				*end += 2;
			else if (line[*end + 1] == ']')
				(*end)++;
		}
		else if (line[*end] == ' ' || line[*end] == '\t')
			break ;
		(*end)++;
	}
	if (in_quotes || in_brackets)
		return (PARSE_ERR_UNCLOSED_BLOCK);
	return (PARSE_OK);
}

t_parse_error	extract_action(char *line, )
{
	return (PARSE_OK);
}

t_parse_error	parse_rule(char *line, t_lex_file *lf)
{
	size_t			ere_end;
	t_parse_error	err;
	size_t			action_start;

	(void)lf;
	err = extract_ere(line, &ere_end);
	if (err != PARSE_OK)
		return (err);
	action_start = ere_end;
	while (line[action_start] && (line[action_start] == ' '
			|| line[action_start] == '\t'))
		action_start++;
	return (PARSE_OK);
}

t_parse_error	parse_line(char *line, t_state *state, t_lex_file *lf)
{
	switch (*state)
	{
	case ST_DEFINITIONS:
		if (!strncmp(line, "%{", 2))
			*state = ST_DECL_BLOCK;
		break ;
	case ST_RULES:
		parse_rule(line, lf);
		break ;
	case ST_USER_CODE:
		break ;
	case ST_DECL_BLOCK:
		if (!strncmp(line, "%}", 2))
		{
			*state = ST_DEFINITIONS;
			break ;
		}
		strbuf_append(&lf->decl_code, line, strlen(line));
		break ;
	}
	return (PARSE_OK);
}

t_parse_error	parse(char *buf, t_lex_file *lf)
{
	char		*line;
	t_cursor	cursor;
	int			err;
	t_state		state;

	state = ST_DEFINITIONS;
	init_cursor(&cursor, buf);
	lex_file_init(lf);
	while (state != ST_USER_CODE)
	{
		err = get_next_cursor_line(&cursor, &line);
		if (err == -1)
			return (PARSE_ERR_ALLOC);
		if (!line)
			break ;
		if (!strncmp(line, "%%", 2))
			state++;
		else
			parse_line(line, &state, lf);
		free(line);
	}
	lf->user_code = cursor.buf + cursor.pos;
	if (state == ST_DEFINITIONS)
		return (PARSE_ERR_NO_SEPARATOR);
	return (PARSE_OK);
}
