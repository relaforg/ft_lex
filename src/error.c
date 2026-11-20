#include "error.h"
#include <stdio.h>

void	print_pattern_error(t_pattern_error err, char *file_name, size_t pos)
{
	(void)pos, (void)file_name;
	printf("pattern_error: %d\n", err);
	switch (err)
	{
	case PAT_OK:
		break ;
	case PAT_ERR_ALLOC:
		break ;
	case PAT_ERR_UNCLOSED_PAREN:
	case PAT_ERR_EMPTY_ALTERNATIVE:
		dprintf(2, "%s:%zu: unrecognized rule\n",
			file_name != NULL ? file_name : "<stdin>", pos);
		break ;
	case PAT_ERR_MISSING_OPERAND:
		break ;
	case PAT_ERR_TOO_DEEP:
		break ;
	case PAT_ERR_TRAILING_BACKSLASH:
		break ;
	case PAT_ERR_UNMATCHED_PAREN:
		dprintf(2, "%s:%zu: unbalanced parenthesis\n",
			file_name != NULL ? file_name : "<stdin>", pos);
		break ;
	case PAT_ERR_UNSUPPORTED:
		break ;
	}
}
