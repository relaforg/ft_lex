#ifndef PARSE_H
# define PARSE_H

#include "lex_file.h"

typedef enum {
	PARSE_OK,
	PARSE_ERR_ALLOC,
	PARSE_ERR_NO_SEPARATOR,
	PARSE_ERR_UNCLOSED_DECL,
	PARSE_ERR_UNCLOSED_QUOTE,
	PARSE_ERR_UNCLOSED_BRACKET,
	PARSE_ERR_UNCLOSED_ACTION,
	PARSE_ERR_NO_ACTION,
	PARSE_ERR_UNSUPPORTED
}	t_parse_error;

t_parse_error parse(char *buf, t_lex_file *lf, size_t *err_line);

#endif
