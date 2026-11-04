#ifndef PARSE_H
# define PARSE_H

typedef enum {
	PARSE_OK,
	PARSE_ERR_ALLOC,
	PARSE_ERR_NO_SEPARATOR,
	PARSE_ERR_UNCLOSED_BLOCK,
	PARSE_ERR_NO_ACTION,
	PARSE_ERR_UNSUPORTED
}	t_parse_error;

t_parse_error parse(char *buf);

#endif
