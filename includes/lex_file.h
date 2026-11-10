#ifndef LEX_FILE_H
# define LEX_FILE_H

#include "strbuf.h"
#include "vec.h"
#include <stddef.h>

typedef struct {
	size_t	id;
	char	*pattern;
	char	*action;
	size_t	line;
}	t_rule;

typedef struct {
	t_strbuf	decl_code;
	t_vec		rules;
	char		*user_code;
}	t_lex_file;

void	lex_file_init(t_lex_file *lf);
void	lex_file_free(t_lex_file *lf);
void	lex_file_dump(t_lex_file *lf);

#endif
