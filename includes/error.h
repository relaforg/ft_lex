#ifndef ERROR_H
# define ERROR_H

# include "parse.h"
# include "pattern.h"
# include <stddef.h>

void	print_parse_error(t_parse_error err, const char *file_name,
			size_t line);
void	print_pattern_error(t_pattern_error err, const char *file_name,
			size_t line);

#endif
