#ifndef GEN_H
# define GEN_H

#include "dfa.h"
#include "lex_file.h"
#include <stdio.h>

int	gen_write(FILE *out, const t_lex_file *lf, const t_dfa *dfa);

#endif
