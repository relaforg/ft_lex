#include "nfa.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Une ligne par etat, dans l'ordre des indices :
 *
 *   nfa: 4 states, start = 1
 *        0  ε     → 3
 *     ▶  1  'a'   → 0
 *     ◉  2              accept r0
 *        3  'b'   → 2
 *
 * ▶ = etat de depart, ◉ = etat acceptant.
 * Un etat ε peut avoir 2 sorties : "→ 4, 7".
 * Le label n'est affiche que si l'etat a au moins une sortie
 * (sinon il n'y a pas de fleche a etiqueter).
 */

#define LABEL_W 6 /* largeur du plus long label : '\x00' */

/* Affiche le label et renvoie sa largeur A L'ECRAN (pas en octets) */
static int	print_label(int c)
{
	if (c == NFA_EPS)
	{
		printf("ε"); /* 2 octets en UTF-8, mais 1 seule colonne */
		return (1);
	}
	if (c == '\n')
		return (printf("'\\n'"));
	if (c == '\t')
		return (printf("'\\t'"));
	if (c == '\'' || c == '\\')
		return (printf("'\\%c'", c));
	if (isprint(c))
		return (printf("'%c'", c));
	return (printf("'\\x%02x'", c));
}

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

static void	print_state(const t_nfa_state *s, int i, int idx_w, int is_start)
{
	int	has_out;
	int	lw;

	if (is_start)
		printf("  ▶ ");
	else if (s->accept != NFA_NULL)
		printf("  ◉ ");
	else
		printf("    ");
	printf("%*d  ", idx_w, i);
	has_out = (s->out1 != NFA_NULL || s->out2 != NFA_NULL);
	lw = 0;
	if (has_out)
		lw = print_label(s->c);
	if (has_out || s->accept != NFA_NULL)
		printf("%*s", LABEL_W - lw, "");
	if (s->out1 != NFA_NULL)
		printf(" → %d", s->out1);
	if (s->out2 != NFA_NULL)
		printf(", %d", s->out2);
	if (s->accept != NFA_NULL)
		printf("   accept r%d", s->accept);
	putchar('\n');
}

/* Version tableau : une ligne par etat, utile pour verifier les indices */
void	nfa_print_table(const t_nfa *nfa, int start)
{
	size_t				i;
	int					idx_w;
	const t_nfa_state	*s;

	printf("  nfa: %zu states, start = %d\n", nfa->states.len, start);
	idx_w = digits(nfa->states.len ? nfa->states.len - 1 : 0);
	i = 0;
	while (i < nfa->states.len)
	{
		s = vec_get(&nfa->states, i);
		print_state(s, (int)i, idx_w, (int)i == start);
		i++;
	}
}

/*
 * Version graphe, de gauche a droite, en partant de l'etat de depart :
 *
 *   (4) ─ε┬→ (1) ─a→ (0) ─ε→ ((5)) r3
 *         └→ (3) ─b→ (2) ─ε→ (5) ⤴
 *
 * (n)   = etat n           ((n)) r3 = etat acceptant pour la regle 3
 * ┬ / └ = un etat a 2 sorties : la 1re continue sur la ligne,
 *         la 2e repart en dessous, alignee
 * ⤴     = etat deja affiche plus haut (jonction d'un ALT ou boucle d'un STAR)
 *
 * Principe : parcours en profondeur. `gutter[col]` vaut 1 tant qu'une
 * branche └ est en attente sous la colonne `col` : chaque nouvelle ligne
 * y dessine un │ pour relier visuellement le ┬ a son └.
 */

typedef struct s_walk
{
	const t_nfa	*nfa;
	char		*seen;
	char		*gutter;
	size_t		cap;
}				t_walk;

/* Label d'une fleche, sans guillemets. Renvoie la largeur a l'ecran */
static int	put_edge_label(int c)
{
	if (c == NFA_EPS)
		return (printf("ε"), 1);
	if (c == ' ')
		return (printf("␣"), 1);
	if (c == '\n')
		return (printf("\\n"));
	if (c == '\t')
		return (printf("\\t"));
	if (isgraph(c))
		return (printf("%c", c));
	return (printf("\\x%02x", c));
}

static int	put_node(const t_nfa_state *s, int i)
{
	if (s->accept != NFA_NULL)
		return (printf("((%d))", i));
	return (printf("(%d)", i));
}

/* Debut de ligne : des espaces, et un │ sous chaque ┬ encore en attente */
static void	put_gutter(const t_walk *w, size_t upto)
{
	size_t	col;

	col = 0;
	while (col < upto)
	{
		if (col < w->cap && w->gutter[col])
			printf("│");
		else
			putchar(' ');
		col++;
	}
}

static void	walk(t_walk *w, int i, size_t col)
{
	const t_nfa_state	*s;
	size_t				k;

	s = vec_get(&w->nfa->states, (size_t)i);
	col += (size_t)put_node(s, i);
	if (w->seen[i])
	{
		printf(" ⤴\n");
		return ;
	}
	w->seen[i] = 1;
	if (s->accept != NFA_NULL)
		col += (size_t)printf(" r%d", s->accept);
	if (s->out1 == NFA_NULL)
	{
		putchar('\n');
		return ;
	}
	printf(" ─");
	k = col + 2 + (size_t)put_edge_label(s->c);
	if (s->out2 == NFA_NULL)
	{
		printf("→ ");
		walk(w, s->out1, k + 2);
		return ;
	}
	printf("┬→ ");
	if (k < w->cap)
		w->gutter[k] = 1;
	walk(w, s->out1, k + 3);
	put_gutter(w, k);
	if (k < w->cap)
		w->gutter[k] = 0;
	printf("└→ ");
	walk(w, s->out2, k + 3);
}

void	nfa_print(const t_nfa *nfa, int start)
{
	t_walk	w;

	printf("  nfa: %zu states\n", nfa->states.len);
	if (start == NFA_NULL || nfa->states.len == 0)
		return ;
	w.nfa = nfa;
	w.cap = nfa->states.len * 32 + 64;
	w.seen = calloc(nfa->states.len, 1);
	w.gutter = calloc(w.cap, 1);
	if (!w.seen || !w.gutter)
		printf("  (nfa_print: alloc error)\n");
	else
	{
		printf("  ");
		walk(&w, start, 2);
	}
	free(w.seen);
	free(w.gutter);
}
