#include "dfa.h"
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
		buf = realloc(buf, capacity);
		if (!buf)
			return (NULL);
		buf = strncat(buf, tmp, read_size);
	}
	return (buf);
}

/* Parse le motif de chaque regle, range l'AST dans `rule->tree`
 * (NULL si le motif est invalide) et l'affiche.
 * Renvoie -1 si au moins un motif est invalide, 0 sinon */
static int	parse_patterns(t_lex_file *lf)
{
	size_t			i;
	t_rule			*rule;
	size_t			err_pos;
	t_pattern_error	err;
	int				status;

	printf("\n-- pattern trees --\n");
	status = 0;
	i = 0;
	while (i < lf->rules.len)
	{
		rule = vec_get(&lf->rules, i);
		printf("[%zu] \"%s\"\n", rule->id, rule->pattern);
		err = pattern_parse(rule->pattern, &rule->tree, &err_pos);
		if (err != PAT_OK)
		{
			printf("  error %d at column %zu\n", err, err_pos);
			status = -1;
		}
		else
			tree_print(rule->tree);
		i++;
	}
	return (status);
}

/* Debug : construit le DFA a partir du NFA fusionne et l'affiche */
static void	dump_dfa(const t_nfa *nfa, int start)
{
	t_dfa	dfa;

	printf("\n-- dfa --\n");
	dfa_init(&dfa, nfa->states.len);
	if (dfa_build(&dfa, nfa, start) == -1)
		printf("  dfa: build failed\n");
	else
		dfa_print(&dfa);
	dfa_free(&dfa);
}

/* Debug : fusionne les NFA de toutes les regles en un seul, l'affiche,
 * puis construit et affiche le DFA correspondant.
 * Suppose que chaque `rule->tree` est non NULL (voir parse_patterns) */
static void	dump_automata(const t_lex_file *lf)
{
	t_nfa	nfa;
	int		start;

	printf("\n-- merged nfa --\n");
	nfa_init(&nfa);
	if (nfa_build_rules(&nfa, &lf->rules, &start) == -1)
		printf("  nfa: build failed (operator not supported yet?)\n");
	else
	{
		nfa_print(&nfa, start);
		dump_dfa(&nfa, start);
	}
	nfa_free(&nfa);
}

int	main(int argc, char **argv)
{
	FILE		*file;
	char		*buf;
	t_lex_file	lf;

	argc--;
	argv++;
	if (argc <= 0)
	{
		file = stdin;
		if (!file)
		{
			dprintf(2, "Error: cannot open stdin\n");
			return (-1);
		}
	}
	else
	{
		file = fopen(argv[0], "r");
		if (!file)
		{
			dprintf(2, "Error: cannot open %s\n", argv[0]);
			return (-1);
		}
	}
	buf = read_fd(file);
	if (!buf)
	{
		dprintf(2, "Error: cannot read\n");
		return (-1);
	}
	if (parse(buf, &lf))
	{
		dprintf(2, "Error: parsing error\n");
		return (-1);
	}
	fclose(file);
	file = fopen("lex.yy.c", "w");
	fclose(file);
	lex_file_dump(&lf);
	if (parse_patterns(&lf) == 0)
		dump_automata(&lf);
	lex_file_free(&lf);
	free(buf);
	return (0);
}
