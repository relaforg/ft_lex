#include "nfa.h"
#include "tree.h"
#include "vec.h"
#include <string.h>

void	nfa_init(t_nfa *nfa)
{
	vec_init(&nfa->states, sizeof(t_nfa_state));
}

void	nfa_free(t_nfa *nfa)
{
	vec_free(&nfa->states, NULL);
}

int	nfa_add_state(t_nfa *nfa, bool eps, int out1, int out2)
{
	t_nfa_state	state;
	int			n;

	state = (t_nfa_state){.out1 = out1, .out2 = out2, .accept = NFA_NULL,
		.eps = eps};
	n = nfa->states.len;
	if (vec_push(&nfa->states, &state) == -1)
		return (-1);
	return (n);
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

	end = nfa_add_state(nfa, true, NFA_NULL, NFA_NULL);
	if (end == -1)
		return (-1);
	start = nfa_add_state(nfa, true, a.start, b.start);
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

	end = nfa_add_state(nfa, true, NFA_NULL, NFA_NULL);
	if (end == -1)
		return (-1);
	start = nfa_add_state(nfa, true, a.start, end);
	if (start == -1)
		return (-1);
	tmp = vec_get(&nfa->states, a.end);
	tmp->out1 = a.start;
	tmp->out2 = end;
	out->start = start;
	out->end = end;
	return (0);
}

static int	nfa_build_class_frag(t_nfa *nfa, t_node *root, t_frag *out)
{
	int			end;
	int			start;
	t_nfa_state	*state;

	end = nfa_add_state(nfa, true, NFA_NULL, NFA_NULL);
	if (end == -1)
		return (-1);
	start = nfa_add_state(nfa, false, end, NFA_NULL);
	if (start == -1)
		return (-1);
	state = vec_get(&nfa->states, start);
	memcpy(state->cset, root->set, sizeof(state->cset));
	out->start = start;
	out->end = end;
	return (0);
}

static int	nfa_build_plus_frag(t_nfa *nfa, t_frag a, t_frag *out)
{
	int			end;
	int			start;
	t_nfa_state	*tmp;

	end = nfa_add_state(nfa, true, NFA_NULL, NFA_NULL);
	if (end == -1)
		return (-1);
	start = nfa_add_state(nfa, true, a.start, NFA_NULL);
	if (start == -1)
		return (-1);
	tmp = vec_get(&nfa->states, a.end);
	tmp->out1 = a.start;
	tmp->out2 = end;
	out->start = start;
	out->end = end;
	return (0);
}

static int	nfa_build_question_mark_frag(t_nfa *nfa, t_frag a, t_frag *out)
{
	int			end;
	int			start;
	t_nfa_state	*tmp;

	end = nfa_add_state(nfa, true, NFA_NULL, NFA_NULL);
	if (end == -1)
		return (-1);
	start = nfa_add_state(nfa, true, a.start, end);
	if (start == -1)
		return (-1);
	tmp = vec_get(&nfa->states, a.end);
	tmp->out1 = end;
	out->start = start;
	out->end = end;
	return (0);
}

static int	nfa_build_node(t_nfa *nfa, t_node *node, t_frag a, t_frag b,
		t_frag *out)
{
	switch (node->type)
	{
	case N_CAT:
		return (nfa_build_cat_frag(nfa, a, b, out));
	case N_ALT:
		return (nfa_build_alt_frag(nfa, a, b, out));
	case N_STAR:
		return (nfa_build_star_frag(nfa, a, out));
	case N_CLASS:
		return (nfa_build_class_frag(nfa, node, out));
	case N_PLUS:
		return (nfa_build_plus_frag(nfa, a, out));
	case N_QUESTION_MARK:
		return (nfa_build_question_mark_frag(nfa, a, out));
	}
}

int	nfa_build(t_nfa *nfa, t_node *root, t_frag *out)
{
	t_frag	a;
	t_frag	b;
	t_vec	stack;
	t_node	*node;

	a = (t_frag){0};
	if (!root)
		return (0);
	vec_init(&stack, sizeof(t_node *));
	for (node = root; node != NULL; node = node->left)
	{
		if (vec_push(&stack, &node) == -1)
		{
			vec_free(&stack, NULL);
			return (-1);
		}
	}
	for (int i = stack.len - 1; i >= 0; i--)
	{
		node = *(t_node **)vec_get(&stack, i);
		if (nfa_build(nfa, node->right, &b) == -1)
		{
			vec_free(&stack, NULL);
			return (-1);
		}
		if (nfa_build_node(nfa, node, a, b, &a) == -1)
		{
			vec_free(&stack, NULL);
			return (-1);
		}
	}
	*out = a;
	vec_free(&stack, NULL);
	return (0);
}
