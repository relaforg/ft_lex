#include "error.h"
#include "pattern.h"
#include <stdio.h>

/* Format commun a toutes les erreurs : "<fichier>:<ligne>: <message>" */
static void	print_error(const char *file_name, size_t line, const char *msg)
{
	dprintf(2, "%s:%zu: %s\n", file_name != NULL ? file_name : "<stdin>", line,
		msg);
}

/* Pas de `default` : avec -Wall -Werror, oublier un code ne compile pas */
void	print_parse_error(t_parse_error err, const char *file_name, size_t line)
{
	switch (err)
	{
	case PARSE_OK:
		break ;
	case PARSE_ERR_ALLOC:
		print_error(file_name, line, "out of memory");
		break ;
	case PARSE_ERR_NO_SEPARATOR:
	case PARSE_ERR_UNCLOSED_DECL:
		print_error(file_name, line, "premature EOF");
		break ;
	case PARSE_ERR_UNCLOSED_QUOTE:
		print_error(file_name, line, "missing quote");
		break ;
	case PARSE_ERR_UNCLOSED_BRACKET:
		print_error(file_name, line, "bad character class");
		break ;
	case PARSE_ERR_UNCLOSED_ACTION:
		print_error(file_name, line, "EOF encountered inside an action");
		break ;
	case PARSE_ERR_NO_ACTION:
		print_error(file_name, line, "missing action");
		break ;
	case PARSE_ERR_UNSUPPORTED:
		print_error(file_name, line, "unsupported construct");
		break ;
	}
}

void	print_pattern_error(t_pattern_error err, const char *file_name,
		size_t line)
{
	switch (err)
	{
	case PAT_OK:
		break ;
	case PAT_ERR_ALLOC:
		print_error(file_name, line, "out of memory");
		break ;
	case PAT_ERR_UNCLOSED_PAREN:
	case PAT_ERR_EMPTY_ALTERNATIVE:
	case PAT_ERR_MISSING_OPERAND:
		print_error(file_name, line, "unrecognized rule");
		break ;
	case PAT_ERR_UNMATCHED_PAREN:
		print_error(file_name, line, "unbalanced parenthesis");
		break ;
	case PAT_ERR_TRAILING_BACKSLASH:
		print_error(file_name, line, "trailing backslash");
		break ;
	case PAT_ERR_TOO_DEEP:
		print_error(file_name, line, "pattern too deeply nested");
		break ;
	case PAT_ERR_UNSUPPORTED:
		print_error(file_name, line, "unsupported pattern construct");
		break ;
	case PAT_ERR_NEGATIVE_RANGE:
		print_error(file_name, line, "negative range");
		break ;
	}
}
