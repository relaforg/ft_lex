#include "nfa.h"
#include "tree.h"
#include "vec.h"

void	nfa_init(t_nfa *nfa)
{
	vec_init(&nfa->states, sizeof(t_nfa_state));
}

void	nfa_free(t_nfa *nfa)
{
	vec_free(&nfa->states, NULL);
}

int	nfa_add_state(t_nfa *nfa, int c, int out1, int out2)
{
	t_nfa_state	state;
	int			n;

	state = (t_nfa_state){.out1 = out1, .out2 = out2, .c = c,
		.accept = NFA_NULL};
	n = nfa->states.len;
	if (vec_push(&nfa->states, &state) == -1)
		return (-1);
	return (n);
}

static int	nfa_build_char_frag(t_nfa *nfa, t_node *root, t_frag *out)
{
	int	end;
	int	start;

	end = nfa_add_state(nfa, NFA_EPS, NFA_NULL, NFA_NULL);
	if (end == -1)
		return (-1);
	start = nfa_add_state(nfa, root->u.c, end, NFA_NULL);
	if (start == -1)
		return (-1);
	out->start = start;
	out->end = end;
	return (0);
}

static int	nfa_build_cat_frag(t_nfa *nfa, t_frag a, t_frag b, t_frag *out)
{
	t_nfa_state	*state;

	state = vec_get(&nfa->states, a.end);
	state->out1 = b.start;
	out->start = a.start;
	out->end = b.end;
	return (0);
}

static int	nfa_build_alt_frag(t_nfa *nfa, t_frag a, t_frag b, t_frag *out)
{
	int			end;
	int			start;
	t_nfa_state	*tmp;

	end = nfa_add_state(nfa, NFA_EPS, NFA_NULL, NFA_NULL);
	if (end == -1)
		return (-1);
	start = nfa_add_state(nfa, NFA_EPS, a.start, b.start);
	if (start == -1)
		return (-1);
	tmp = vec_get(&nfa->states, a.end);
	tmp->out1 = end;
	tmp = vec_get(&nfa->states, b.end);
	tmp->out1 = end;
	out->start = start;
	out->end = end;
	return (0);
}

static int	nfa_build_star_frag(t_nfa *nfa, t_frag a, t_frag *out)
{
	int			end;
	int			start;
	t_nfa_state	*tmp;

	end = nfa_add_state(nfa, NFA_EPS, NFA_NULL, NFA_NULL);
	if (end == -1)
		return (-1);
	start = nfa_add_state(nfa, NFA_EPS, a.start, end);
	if (start == -1)
		return (-1);
	tmp = vec_get(&nfa->states, a.end);
	tmp->out1 = a.start;
	tmp->out2 = end;
	out->start = start;
	out->end = end;
	return (0);
}

int	nfa_build(t_nfa *nfa, t_node *root, t_frag *out)
{
	t_frag	a;
	t_frag	b;

	if (!root)
		return (0);
	if (nfa_build(nfa, root->left, &a) == -1)
		return (-1);
	if (nfa_build(nfa, root->right, &b) == -1)
		return (-1);
	switch (root->type)
	{
	case N_CHAR:
		return (nfa_build_char_frag(nfa, root, out));
	case N_CAT:
		return (nfa_build_cat_frag(nfa, a, b, out));
	case N_ALT:
		return (nfa_build_alt_frag(nfa, a, b, out));
	case N_STAR:
		return (nfa_build_star_frag(nfa, a, out));
	case N_PLUS:
		return (-1);
	}
	return (-1);
}
