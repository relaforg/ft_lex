#ifndef DFA_H
# define DFA_H

#include "nfa.h"
# include "vec.h"
# include <stdint.h>

# define DFA_NCHARS 256

typedef struct
{
	uint64_t	*set;
	int			trans[DFA_NCHARS];
	int			accept;
}	t_dfa_state;

typedef struct
{
	t_vec	states;
	size_t	nwords;
}	t_dfa;

void	dfa_init(t_dfa *dfa, size_t nfa_len);
void	dfa_free(t_dfa *dfa);
int		dfa_build(t_dfa *dfa, const t_nfa *nfa, int start);
void	dfa_print(const t_dfa *dfa);

#endif
