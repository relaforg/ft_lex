#include "bitset.h"
#include "dfa.h"
#include "nfa.h"
#include "vec.h"
#include <stdlib.h>
#include <string.h>

void	dfa_init(t_dfa *dfa, size_t nfa_len)
{
	vec_init(&dfa->states, sizeof(t_dfa_state));
	dfa->nwords = bitset_nwords(nfa_len);
}

void	dfa_state_free(void *vstate)
{
	t_dfa_state	*state;

	state = vstate;
	free(state->set);
	state->set = NULL;
	state->accept = NFA_NULL;
}

void	dfa_free(t_dfa *dfa)
{
	vec_free(&dfa->states, dfa_state_free);
	dfa->nwords = 0;
}

static int	rule_of_set(const t_nfa *nfa, const uint64_t *set)
{
	int					best;
	size_t				i;
	const t_nfa_state	*state;

	best = NFA_NULL;
	i = 0;
	while (i < nfa->states.len)
	{
		state = vec_get(&nfa->states, i);
		if (state->accept != NFA_NULL && bitset_test(set, i)
			&& (best <= NFA_NULL || best > state->accept))
			best = state->accept;
		i++;
	}
	return (best);
}

static int	add_state(t_dfa *dfa, const t_nfa *nfa, uint64_t *set)
{
	t_dfa_state	state;

	state.set = set;
	for (int c = 0; c < DFA_NCHARS; c++)
		state.trans[c] = NFA_NULL;
	state.accept = rule_of_set(nfa, state.set);
	if (vec_push(&dfa->states, &state) == -1)
		return (-1);
	return (dfa->states.len - 1);
}

static void	push_if_new(uint64_t *set, int *stack, int *top, int s)
{
	if (s != NFA_NULL && !bitset_test(set, s))
	{
		stack[(*top)++] = s;
		bitset_set(set, s);
	}
}

static void	eps_closure(const t_nfa *nfa, uint64_t *set, int *stack)
{
	int					top;
	const t_nfa_state	*state;

	top = 0;
	for (size_t s = 0; s < nfa->states.len; s++)
		if (bitset_test(set, s))
			stack[top++] = s;
	while (top > 0)
	{
		state = vec_get(&nfa->states, stack[--top]);
		if (state->c != NFA_EPS)
			continue ;
		push_if_new(set, stack, &top, state->out1);
		push_if_new(set, stack, &top, state->out2);
	}
}

static uint64_t	*move_on(const t_nfa *nfa, const uint64_t *set, size_t nwords,
		int c)
{
	uint64_t	*next;
	t_nfa_state	*tmp;

	next = bitset_new(nwords);
	if (!next)
		return (NULL);
	for (size_t s = 0; s < nfa->states.len; s++)
	{
		if (!bitset_test(set, s))
			continue ;
		tmp = vec_get(&nfa->states, s);
		if (tmp->c == c)
			bitset_set(next, tmp->out1);
	}
	return (next);
}

static int	find_state(const t_dfa *dfa, const uint64_t *set)
{
	t_dfa_state	*tmp;

	for (size_t i = 0; i < dfa->states.len; i++)
	{
		tmp = vec_get(&dfa->states, i);
		if (bitset_eq(tmp->set, set, dfa->nwords))
			return (i);
	}
	return (NFA_NULL);
}

static int	build_transitions(t_dfa *dfa, const t_nfa *nfa, size_t i,
		int *stack)
{
	t_dfa_state	*curr;
	uint64_t	*next;
	int			j;

	for (int c = 0; c < 256; c++)
	{
		curr = vec_get(&dfa->states, i);
		next = move_on(nfa, curr->set, dfa->nwords, c);
		if (!next)
			return (-1);
		if (bitset_is_empty(next, dfa->nwords))
		{
			free(next);
			continue ;
		}
		eps_closure(nfa, next, stack);
		j = find_state(dfa, next);
		if (j != NFA_NULL)
			free(next);
		else
		{
			j = add_state(dfa, nfa, next);
			if (j == -1)
			{
				free(next);
				return (-1);
			}
		}
		curr = vec_get(&dfa->states, i);
		curr->trans[c] = j;
	}
	return (0);
}

int	dfa_build(t_dfa *dfa, const t_nfa *nfa, int start)
{
	int			*stack;
	uint64_t	*set;
	t_dfa_state	*tmp;
	size_t		i;

	if (start == NFA_NULL)
		return (0);
	stack = malloc(nfa->states.len * sizeof(int));
	if (!stack)
		return (-1);
	set = bitset_new(dfa->nwords);
	if (!set)
	{
		free(stack);
		return (-1);
	}
	bitset_set(set, start);
	eps_closure(nfa, set, stack);
	if (add_state(dfa, nfa, set) == -1)
	{
		free(stack);
		free(set);
		return (-1);
	}
	i = 0;
	while (i < dfa->states.len)
	{
		if (build_transitions(dfa, nfa, i, stack) == -1)
		{
			free(stack);
			return (-1);
		}
		i++;
	}
	tmp = vec_get(&dfa->states, 0);
	tmp->accept = NFA_NULL;
	free(stack);
	return (0);
}
