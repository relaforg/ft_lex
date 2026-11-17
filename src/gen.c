#include "dfa.h"
#include "gen.h"
#include "vec.h"
#include <stdio.h>

static void	emit_prologue(FILE *out)
{
	fputs("#include <stdio.h>\n"
			"#include <stdlib.h>\n\n"
			"char\t*yytext;\n"
			"int\tyyleng;"
			"\nFILE\t*yyin;"
			"\nFILE\t*yyout;"
			"\n\nint\tyywrap(void);\n\n"
			"#define ECHO fwrite(yytext,(size_t)yyleng,1,yyout)\n",
			out);
}

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

static void	emit_yylex_top(FILE *out)
{
	fputs("\n"
			"static char\t*yy_buf;\n"
			"static size_t\tyy_len;\n"
			"static size_t\tyy_pos;\n"
			"static char\tyy_hold;\n"
			"\n"
			"static int\tyy_load(void)\n"
			"{\n"
			"\tsize_t\tcap;\n"
			"\tsize_t\tn;\n"
			"\tchar\t*tmp;\n"
			"\n"
			"\tif (!yyin)\n"
			"\t\tyyin = stdin;\n"
			"\tif (!yyout)\n"
			"\t\tyyout = stdout;\n"
			"\tcap = 4096;\n"
			"\tyy_len = 0;\n"
			"\tyy_buf = malloc(cap);\n"
			"\tif (!yy_buf)\n"
			"\t{\n"
			"\t\tfputs(\"yylex: out of memory\\n\", stderr);\n"
			"\t\treturn (-1);\n"
			"\t}\n"
			"\twhile ((n = fread(yy_buf + yy_len, 1, cap - yy_len - 1,"
			"\t\tyyin)) > 0)\n"
			"\t{\n"
			"\t\tyy_len += n;\n"
			"\t\tif (yy_len + 1 >= cap)\n"
			"\t\t{\n"
			"\t\t\tcap *= 2;\n"
			"\t\t\ttmp = realloc(yy_buf, cap);\n"
			"\t\t\tif (!tmp)\n"
			"\t\t\t{\n"
			"\t\t\t\tfree(yy_buf);\n"
			"\t\t\t\tyy_buf = NULL;\n"
			"\t\t\t\tfputs(\"yylex: out of memory\\n\", stderr);\n"
			"\t\t\t\treturn (-1);\n"
			"\t\t\t}\n"
			"\t\t\tyy_buf = tmp;\n"
			"\t\t}\n"
			"\t}\n"
			"\tyy_buf[yy_len] = '\\0';\n"
			"\tyy_pos = 0;\n"
			"\tyy_hold = yy_buf[0];\n"
			"\treturn (0);\n"
			"}\n"
			"\n"
			"int\tyylex(void)\n"
			"{\n"
			"\tint\t\tstate;\n"
			"\tint\t\tyy_rule;\n"
			"\tsize_t\tj;\n"
			"\tsize_t\tend;\n"
			"\n"
			"\tif (!yy_buf && yy_load() == -1)\n"
			"\t\treturn (0);\n"
			"\twhile (1)\n"
			"\t{\n"
			"\t\tyy_buf[yy_pos] = yy_hold;\n"
			"\t\tif (yy_pos == yy_len)\n"
			"\t\t{\n"
			"\t\t\tfree(yy_buf);\n"
			"\t\t\tyy_buf = NULL;\n"
			"\t\t\tif (yywrap() || yy_load() == -1)\n"
			"\t\t\t\treturn (0);\n"
			"\t\t\tcontinue ;\n"
			"\t\t}\n"
			"\t\tstate = 0;\n"
			"\t\tyy_rule = 0;\n"
			"\t\tend = yy_pos;\n"
			"\t\tj = yy_pos;\n"
			"\t\twhile (j < yy_len)\n"
			"\t\t{\n"
			"\t\t\tstate = yy_nxt[state][yy_ec[(unsigned char)yy_buf[j]]];\n"
			"\t\t\tif (state == -1)\n"
			"\t\t\t\tbreak ;\n"
			"\t\t\tj++;\n"
			"\t\t\tif (yy_accept[state])\n"
			"\t\t\t{\n"
			"\t\t\t\tyy_rule = yy_accept[state];\n"
			"\t\t\t\tend = j;\n"
			"\t\t\t}\n"
			"\t\t}\n"
			"\t\tif (yy_rule == 0)\n"
			"\t\t\tend = yy_pos + 1;\n"
			"\t\tyy_hold = yy_buf[end];\n"
			"\t\tyy_buf[end] = '\\0';\n"
			"\t\tyytext = yy_buf + yy_pos;\n"
			"\t\tyyleng = (int)(end - yy_pos);\n"
			"\t\tyy_pos = end;\n"
			"\t\tswitch (yy_rule)\n"
			"\t\t{\n"
			"\t\tcase 0:\n"
			"\t\t\tECHO;\n"
			"\t\t\tbreak ;\n",
			out);
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

static void	emit_yylex_bottom(FILE *out)
{
	fputs("\t\t}\n"
			"\t}\n"
			"}\n"
			"\n",
			out);
}

static void	emit_raw(FILE *out, char *raw)
{
	if (raw)
		fputs(raw, out);
}

int	gen_write(FILE *out, const t_lex_file *lf, const t_dfa *dfa)
{
	emit_prologue(out);
	emit_raw(out, lf->decl_code.data);
	emit_tables(out, dfa);
	emit_yylex_top(out);
	emit_cases(out, lf);
	emit_yylex_bottom(out);
	emit_raw(out, lf->user_code);
	return (0);
}
