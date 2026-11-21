#include "lex_file.h"
#include "nfa.h"
#include "vec.h"

int	nfa_build_rules(t_nfa *nfa, const t_vec *rules, int *start)
{
	t_frag		t;
	t_nfa_state	*tmp;
	t_rule		*rule;
	int			err;

	t.start = NFA_NULL;
	t.end = NFA_NULL;
	*start = NFA_NULL;
	for (int i = rules->len - 1; i >= 0; i--)
	{
		rule = vec_get(rules, i);
		if (!rule || !rule->tree)
			return (-1);
		if (nfa_build(nfa, rule->tree, &t) == -1)
			return (-1);
		tmp = vec_get(&nfa->states, t.end);
		if (!tmp)
			return (-1);
		tmp->accept = rule->id;
		err = nfa_add_state(nfa, NFA_EPS, t.start, *start);
		if (err == -1)
			return (-1);
		*start = err;
	}
	return (0);
}
