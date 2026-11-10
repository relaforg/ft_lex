#include "cursor.h"
#include "lex_file.h"
#include "parse.h"
#include "strbuf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
	ST_DEFINITIONS = 0,
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

t_parse_error	extract_ere(t_cursor *cursor, char **pattern)
{
	char	in_quotes;
	char	in_brackets;
	size_t	start;

	in_quotes = 0;
	in_brackets = 0;
	start = cursor->pos;
	while (get_cursor_char(cursor) && get_cursor_char(cursor) != '\n')
	{
		if (get_cursor_char(cursor) == '\\' && cursor->buf[cursor->pos + 1]
			&& cursor->buf[cursor->pos + 1] != '\n')
			cursor_advance(cursor);
		else if (in_quotes)
			in_quotes = (get_cursor_char(cursor) != '"');
		else if (in_brackets)
			in_brackets = (get_cursor_char(cursor) != ']');
		else if (get_cursor_char(cursor) == '"')
			in_quotes = 1;
		else if (get_cursor_char(cursor) == '[')
		{
			in_brackets = 1;
			if (cursor->buf[cursor->pos + 1] && cursor->buf[cursor->pos
				+ 1] == '^' && cursor->buf[cursor->pos + 2]
				&& cursor->buf[cursor->pos + 2] == ']')
			{
				cursor_advance(cursor);
				cursor_advance(cursor);
			}
			else if (cursor->buf[cursor->pos + 1] == ']')
				cursor_advance(cursor);
		}
		else if (get_cursor_char(cursor) == ' '
			|| get_cursor_char(cursor) == '\t')
			break ;
		cursor_advance(cursor);
	}
	if (in_quotes || in_brackets)
		return (PARSE_ERR_UNCLOSED_BLOCK);
	*pattern = strndup(cursor->buf + start, cursor->pos - start);
	if (!*pattern)
		return (PARSE_ERR_ALLOC);
	return (PARSE_OK);
}

t_parse_error	extract_action(t_cursor *cursor)
{
	cursor_skip_line(cursor);
	return (PARSE_OK);
}

t_parse_error	parse_rule(t_cursor *cursor, t_lex_file *lf)
{
	char			*pattern;
	t_parse_error	err;

	(void)lf;
	err = extract_ere(cursor, &pattern);
	if (err != PARSE_OK)
		return (err);
	cursor_skip_blanks(cursor);
	err = extract_action(cursor);
	if (err != PARSE_OK)
	{
		free(pattern);
		return (err);
	}
	free(pattern);
	return (PARSE_OK);
}

t_parse_error	parse_declaration(t_cursor *cursor, t_lex_file *lf)
{
	char	*line;
	int		err;

	if (cursor_starts_with(cursor, "%{"))
	{
		cursor_skip_line(cursor);
		while (1)
		{
			if (cursor_starts_with(cursor, "%}"))
			{
				cursor_skip_line(cursor);
				break ;
			}
			err = get_next_cursor_line(cursor, &line);
			if (err == -1)
				return (PARSE_ERR_ALLOC);
			if (!line)
				return (PARSE_ERR_UNCLOSED_BLOCK);
			strbuf_append(&lf->decl_code, line, strlen(line));
			free(line);
		}
	}
	else
		cursor_skip_line(cursor);
	return (PARSE_OK);
}

t_parse_error	parse_dispach(t_cursor *cursor, t_state *state, t_lex_file *lf)
{
	switch (*state)
	{
	case ST_DEFINITIONS:
		return (parse_declaration(cursor, lf));
	case ST_RULES:
		return (parse_rule(cursor, lf));
	case ST_USER_CODE:
		break ;
	}
	return (PARSE_OK);
}

t_parse_error	parse(char *buf, t_lex_file *lf)
{
	t_cursor		cursor;
	t_state			state;
	t_parse_error	err;

	state = ST_DEFINITIONS;
	init_cursor(&cursor, buf);
	lex_file_init(lf);
	while (state != ST_USER_CODE && get_cursor_char(&cursor))
	{
		if (cursor_starts_with(&cursor, "%%"))
		{
			cursor_skip_line(&cursor);
			state++;
		}
		else
		{
			err = parse_dispach(&cursor, &state, lf);
			if (err != PARSE_OK)
				return (err);
		}
	}
	lf->user_code = cursor.buf + cursor.pos;
	if (state == ST_DEFINITIONS)
		return (PARSE_ERR_NO_SEPARATOR);
	return (PARSE_OK);
}
