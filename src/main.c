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

/* Debug : construit le NFA d'un AST, marque la sortie comme acceptante
 * pour la regle `rule_id`, puis l'affiche */
static void	dump_nfa(t_node *tree, size_t rule_id)
{
	t_nfa		nfa;
	t_frag		f;
	t_nfa_state	*end;

	nfa_init(&nfa);
	if (nfa_build(&nfa, tree, &f) == -1)
		printf("  nfa: build failed (operator not supported yet?)\n");
	else
	{
		end = vec_get(&nfa.states, f.end);
		end->accept = (int)rule_id;
		nfa_print(&nfa, f.start);
	}
	nfa_free(&nfa);
}

/* Debug : parse le motif de chaque regle et affiche son AST + son NFA */
static void	dump_pattern_trees(const t_lex_file *lf)
{
	size_t			i;
	t_rule			*rule;
	t_node			*tree;
	size_t			err_pos;
	t_pattern_error	err;

	printf("\n-- pattern trees --\n");
	i = 0;
	while (i < lf->rules.len)
	{
		rule = vec_get(&lf->rules, i);
		printf("[%zu] \"%s\"\n", rule->id, rule->pattern);
		err = pattern_parse(rule->pattern, &tree, &err_pos);
		if (err != PAT_OK)
			printf("  error %d at column %zu\n", err, err_pos);
		else
		{
			tree_print(tree);
			dump_nfa(tree, rule->id);
			tree_free(tree);
		}
		i++;
	}
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
	dump_pattern_trees(&lf);
	lex_file_free(&lf);
	free(buf);
	return (0);
}
