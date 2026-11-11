#ifndef PATTERN_H
# define PATTERN_H

# include "tree.h"
# include <stddef.h>

# define PATTERN_MAX_DEPTH 1000

typedef enum
{
	PAT_OK,
	PAT_ERR_ALLOC,
	PAT_ERR_UNCLOSED_PAREN,
	PAT_ERR_UNMATCHED_PAREN,
	PAT_ERR_MISSING_OPERAND,
	PAT_ERR_EMPTY_ALTERNATIVE,
	PAT_ERR_TRAILING_BACKSLASH,
	PAT_ERR_UNSUPPORTED,
	PAT_ERR_TOO_DEEP
}	t_pattern_error;

typedef struct
{
	const char		*s;
	size_t			pos;
	size_t			depth;
	t_pattern_error	err;
}	t_pcur;

void			pcur_init(t_pcur *p, const char *s);
unsigned char	pcur_peek(const t_pcur *p);
void			pcur_advance(t_pcur *p);
int				pcur_match(t_pcur *p, char c);
t_node			*pcur_fail(t_pcur *p, t_pattern_error err);
int				pcur_enter(t_pcur *p);
void			pcur_leave(t_pcur *p);

t_pattern_error	pattern_parse(const char *pattern, t_node **out,
					size_t *err_pos);
const char		*pattern_strerror(t_pattern_error err);
t_node			*parse_alt(t_pcur *p);
t_node			*parse_cat(t_pcur *p);
t_node			*parse_star(t_pcur *p);
t_node			*parse_atom(t_pcur *p);

#endif
