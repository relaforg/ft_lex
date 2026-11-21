#include "bitset.h"
#include "dfa.h"
#include <ctype.h>
#include <stdio.h>

/*
 * Une ligne par etat DFA, dans l'ordre des indices :
 *
 *   dfa: 3 states
 *   ▶ 0  {1,2,3,5,6}  'a'→1 'b'→2
 *     1  {0,1,2,5}    'a'→1 'b'→2
 *   ◉ 2  {4}          accept r0
 *
 * ▶ = etat de depart (toujours l'etat 0), ◉ = etat acceptant.
 * {...} = ensemble des etats NFA que represente cet etat DFA.
 * Seules les transitions existantes sont affichees ; les caracteres
 * consecutifs qui menent au meme etat sont regroupes : '0'-'9'→1.
 */

typedef struct s_cols
{
	int	idx;
	int	set;
}		t_cols;

static int	digits(size_t n)
{
	int	d;

	d = 1;
	while (n >= 10)
	{
		n /= 10;
		d++;
	}
	return (d);
}

/* Meme rendu que les labels de nfa_print_table : 'a', '\n', '\x01'... */
static void	put_char(int c)
{
	if (c == '\n')
		printf("'\\n'");
	else if (c == '\t')
		printf("'\\t'");
	else if (c == '\'' || c == '\\')
		printf("'\\%c'", c);
	else if (isprint(c))
		printf("'%c'", c);
	else
		printf("'\\x%02x'", c);
}

/* Affiche {1,2,3} et renvoie le nombre de colonnes utilisees */
static int	print_set(const uint64_t *set, size_t nwords)
{
	size_t	i;
	int		w;
	int		first;

	w = printf("{");
	first = 1;
	i = 0;
	while (i < nwords * 64)
	{
		if (bitset_test(set, i))
		{
			if (!first)
				w += printf(",");
			w += printf("%zu", i);
			first = 0;
		}
		i++;
	}
	return (w + printf("}"));
}

/* Largeur qu'occuperait print_set, sans rien afficher */
static int	set_width(const uint64_t *set, size_t nwords)
{
	size_t	i;
	int		w;
	int		count;

	w = 2;
	count = 0;
	i = 0;
	while (i < nwords * 64)
	{
		if (bitset_test(set, i))
		{
			w += digits(i) + (count > 0);
			count++;
		}
		i++;
	}
	return (w);
}

/* Parcourt les 256 transitions et regroupe les plages consecutives
 * qui menent au meme etat */
static void	print_transitions(const t_dfa_state *s)
{
	int	c;
	int	end;

	c = 0;
	while (c < DFA_NCHARS)
	{
		if (s->trans[c] == NFA_NULL)
		{
			c++;
			continue ;
		}
		end = c;
		while (end + 1 < DFA_NCHARS && s->trans[end + 1] == s->trans[c])
			end++;
		putchar(' ');
		put_char(c);
		if (end > c)
		{
			putchar('-');
			put_char(end);
		}
		printf("→%d", s->trans[c]);
		c = end + 1;
	}
}

static void	print_state(const t_dfa *dfa, size_t i, const t_cols *cols)
{
	const t_dfa_state	*s;
	int					w;

	s = vec_get(&dfa->states, i);
	if (i == 0)
		printf("  ▶ ");
	else if (s->accept != NFA_NULL)
		printf("  ◉ ");
	else
		printf("    ");
	printf("%*zu  ", cols->idx, i);
	w = print_set(s->set, dfa->nwords);
	printf("%*s", cols->set - w, "");
	print_transitions(s);
	if (s->accept != NFA_NULL)
		printf("   accept r%d", s->accept);
	putchar('\n');
}

void	dfa_print(const t_dfa *dfa)
{
	t_cols				cols;
	const t_dfa_state	*s;
	size_t				i;
	int					w;

	printf("  dfa: %zu states\n", dfa->states.len);
	if (dfa->states.len == 0)
		return ;
	cols.idx = digits(dfa->states.len - 1);
	cols.set = 0;
	i = 0;
	while (i < dfa->states.len)
	{
		s = vec_get(&dfa->states, i);
		w = set_width(s->set, dfa->nwords);
		if (w > cols.set)
			cols.set = w;
		i++;
	}
	i = 0;
	while (i < dfa->states.len)
	{
		print_state(dfa, i, &cols);
		i++;
	}
}
