#ifndef NFA_H
# define NFA_H

# include "tree.h"
# include "vec.h"
# include <stdint.h>
# include <stdbool.h>

# define NFA_NULL (-1)
# define NFA_CSET_NWORDS 4

typedef struct {
	uint64_t	cset[NFA_CSET_NWORDS];
	int			out1;
	int			out2;
	int			accept;
	bool		eps;
}	t_nfa_state;

typedef struct {
	int	start;
	int end;
}	t_frag;

typedef struct {
	t_vec	states;
}	t_nfa;

int		nfa_add_state(t_nfa *nfa, bool eps, int out1, int out2);
void	nfa_init(t_nfa *nfa);
void	nfa_free(t_nfa *nfa);
int		nfa_build(t_nfa *nfa, t_node *root, t_frag *out);
int		nfa_build_rules(t_nfa *nfa, const t_vec *rules, int *start);
void	nfa_print(const t_nfa *nfa, int start);
void	nfa_print_table(const t_nfa *nfa, int start);

#endif
