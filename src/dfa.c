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
	int			best;
	size_t		i;
	t_nfa_state	*state;

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

static void	eps_closure(const t_nfa *nfa, uint64_t *set, int *stack)
{
	int	top;

	top = 0;
	for (size_t s = 0; s < nfa->states.len; s++)
		if (bitset_test(set, s))
			stack[top++] = s;
}

int			dfa_build(t_dfa *dfa, const t_nfa *nfa, int start);
