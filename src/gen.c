#include "dfa.h"
#include "gen.h"
#include "vec.h"
#include <stdio.h>
#include <string.h>

extern unsigned char	lex_skel[];
extern unsigned int		lex_skel_len;

static void	emit_nxt(FILE *out, const t_dfa *dfa, size_t n, int nclasses,
		int rep[DFA_NCHARS])
{
	int			v;
	t_dfa_state	*state;

	fprintf(out, "static const int\tyy_nxt[%zu][%d] = {\n", n, nclasses);
	for (size_t s = 0; s < n; s++)
	{
		fprintf(out, "\t{/* state %zu */\n\t", s);
		for (int c = 0; c < nclasses; c++)
		{
			if (s < dfa->states.len)
			{
				state = vec_get(&dfa->states, s);
				v = state->trans[rep[c]];
			}
			else
				v = -1;
			fprintf(out, "%d,", v);
			if (c % 16 == 15)
				fputs("\n", out);
			else
				fputs(" ", out);
		}
		fputs("},\n", out);
	}
	fputs("};\n\n", out);
}

static void	emit_accept(FILE *out, const t_dfa *dfa, size_t n)
{
	t_dfa_state	*state;

	fprintf(out, "static const int\tyy_accept[%zu] = { ", n);
	for (size_t s = 0; s < n; s++)
	{
		if (s < dfa->states.len)
		{
			state = vec_get(&dfa->states, s);
			fprintf(out, "%d, ", state->accept + 1);
		}
		else
			fputs("0, ", out);
	}
	fputs("};\n\n", out);
}

static void	emit_ec(FILE *out, int ec[DFA_NCHARS])
{
	fputs("static const\tint yy_ec[256] = {", out);
	for (size_t i = 0; i < DFA_NCHARS; i++)
	{
		fprintf(out, "%d, ", ec[i]);
	}
	fputs("};\n", out);
}

static void	emit_tables(FILE *out, const t_dfa *dfa)
{
	size_t	n;
	int		ec[DFA_NCHARS] = {0};
	int		rep[DFA_NCHARS] = {0};
	int		nclasses;

	n = dfa->states.len;
	if (n == 0)
		n = 1;
	nclasses = dfa_equiv_classes(dfa, ec, rep);
	emit_ec(out, ec);
	emit_nxt(out, dfa, n, nclasses, rep);
	emit_accept(out, dfa, n);
}

static void	emit_cases(FILE *out, const t_lex_file *lf)
{
	t_rule	*rule;

	for (size_t i = 0; i < lf->rules.len; i++)
	{
		rule = vec_get(&lf->rules, i);
		fprintf(out, "\tcase %zu:\n\t\t{", rule->id + 1);
		fputs(rule->action, out);
		fputs(" }\n\t\tbreak ;\n", out);
	}
}

int	gen_write(FILE *out, const t_lex_file *lf, const t_dfa *dfa)
{
	size_t	start;
	size_t	i;

	i = 0;
	start = 0;
	while (i < lex_skel_len)
	{
		if (!strncmp((const char *)(lex_skel + i), "%% ", 3))
		{
			fwrite((const char *)(lex_skel + start), sizeof(unsigned char), i
				- start, out);
			i += 3;
			if (!strncmp((const char *)(lex_skel + i), "decl", 4))
				fwrite(lf->decl_code.data, sizeof(char), lf->decl_code.len,
					out);
			else if (!strncmp((const char *)(lex_skel + i), "tables", 6))
				emit_tables(out, dfa);
			else if (!strncmp((const char *)(lex_skel + i), "cases", 5))
				emit_cases(out, lf);
			else if (!strncmp((const char *)(lex_skel + i), "user", 4))
				fwrite(lf->user_code, sizeof(char), strlen(lf->user_code), out);
			while (i < lex_skel_len && *(lex_skel + i) != '\n')
				i++;
			start = i + 1;
			continue ;
		}
		i++;
	}
	fwrite((const char *)(lex_skel + start), sizeof(unsigned char), i - start,
		out);
	return (ferror(out) ? -1 : 0);
}
