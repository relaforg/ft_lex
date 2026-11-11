#ifndef NFA_H
# define NFA_H

# include "tree.h"
# include "vec.h"

# define NFA_EPS 256
# define NFA_NULL (-1)

typedef struct {
	int	c;
	int	out1;
	int	out2;
	int	accept;
}	t_nfa_state;

typedef struct {
	int	start;
	int end;
}	t_frag;

typedef struct {
	t_vec	states;
}	t_nfa;

int		nfa_add_state(t_nfa *nfa, int c, int out1, int out2);
void	nfa_init(t_nfa *nfa);
void	nfa_free(t_nfa *nfa);
int		nfa_build(t_nfa *nfa, t_node *root, t_frag *out);
void	nfa_print(const t_nfa *nfa, int start);
void	nfa_print_table(const t_nfa *nfa, int start);

#endif
