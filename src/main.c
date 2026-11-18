#include "dfa.h"
#include "gen.h"
#include "lex_file.h"
#include "nfa.h"
#include "parse.h"
#include "pattern.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ_SIZE 1024

char	*read_fd(FILE *file)
{
	size_t	capacity;
	char	*buf;
	size_t	read_size;
	char	tmp[READ_SIZE];
	char	*t;

	capacity = READ_SIZE;
	buf = calloc(1, sizeof(*buf));
	if (!buf)
		return (NULL);
	while (1)
	{
		read_size = fread(tmp, sizeof(*tmp), READ_SIZE, file);
		if (!read_size)
			break ;
		capacity += READ_SIZE;
		t = realloc(buf, capacity);
		if (!t)
		{
			free(buf);
			return (NULL);
		}
		buf = t;
		buf = strncat(buf, tmp, read_size);
	}
	return (buf);
}

/* Lit tout le fichier `path` (stdin si NULL). Renvoie NULL en cas d'erreur */
static char	*read_input(const char *path)
{
	FILE	*file;
	char	*buf;

	file = stdin;
	if (path)
		file = fopen(path, "r");
	if (!file)
	{
		dprintf(2, "Error: cannot open %s\n", path);
		return (NULL);
	}
	buf = read_fd(file);
	if (file != stdin)
		fclose(file);
	if (!buf)
		dprintf(2, "Error: cannot read input\n");
	return (buf);
}

/* Parse le motif de chaque regle et range l'AST dans `rule->tree`.
 * Renvoie -1 si au moins un motif est invalide, 0 sinon */
static int	parse_patterns(t_lex_file *lf)
{
	size_t			i;
	t_rule			*rule;
	size_t			err_pos;
	t_pattern_error	err;
	int				status;

	status = 0;
	i = 0;
	while (i < lf->rules.len)
	{
		rule = vec_get(&lf->rules, i);
		err = pattern_parse(rule->pattern, &rule->tree, &err_pos);
		if (err != PAT_OK)
		{
			dprintf(2,
					"Error: rule %zu: invalid pattern (error %d at column"
					" %zu)\n ",
					rule->id,
					err,
					err_pos);
			status = -1;
		}
		i++;
	}
	return (status);
}

/* Construit le DFA a partir des regles, via un NFA intermediaire.
 * Renvoie 0 si `dfa` est construit (a liberer avec dfa_free), -1 sinon */
static int	build_dfa(const t_lex_file *lf, t_dfa *dfa)
{
	t_nfa	nfa;
	int		start;
	int		status;

	nfa_init(&nfa);
	status = nfa_build_rules(&nfa, &lf->rules, &start);
	if (status == 0)
	{
		dfa_init(dfa, nfa.states.len);
		status = dfa_build(dfa, &nfa, start);
		if (status != 0)
			dfa_free(dfa);
	}
	nfa_free(&nfa);
	if (status != 0)
		dprintf(2, "Error: cannot build automata\n");
	return (status);
}

/* Ecrit le scanner genere dans lex.yy.c. Renvoie 0 ou -1 */
static int	write_output(const t_lex_file *lf, const t_dfa *dfa)
{
	FILE	*out;
	int		status;

	out = fopen("lex.yy.c", "w");
	if (!out)
	{
		dprintf(2, "Error: cannot open lex.yy.c\n");
		return (-1);
	}
	status = gen_write(out, lf, dfa);
	if (fclose(out) != 0)
		status = -1;
	return (status);
}

/* Patterns -> DFA -> lex.yy.c. Renvoie 0 ou -1 */
static int	generate(t_lex_file *lf)
{
	t_dfa	dfa;
	int		status;

	if (parse_patterns(lf) != 0 || build_dfa(lf, &dfa) != 0)
		return (-1);
	status = write_output(lf, &dfa);
	dfa_free(&dfa);
	return (status);
}

int	main(int argc, char **argv)
{
	char		*buf;
	t_lex_file	lf;
	int			status;

	buf = read_input(argc > 1 ? argv[1] : NULL);
	if (!buf)
		return (1);
	status = -1;
	if (parse(buf, &lf) != PARSE_OK)
		dprintf(2, "Error: parsing error\n");
	else
		status = generate(&lf);
	lex_file_free(&lf);
	free(buf);
	return (status != 0);
}
