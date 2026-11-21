#include "lex_file.h"
#include "strbuf.h"
#include "tree.h"
#include "vec.h"
#include <stdio.h>
#include <stdlib.h>

void	lex_file_init(t_lex_file *lf)
{
	strbuf_init(&lf->decl_code);
	vec_init(&lf->rules, sizeof(t_rule));
	lf->user_code = NULL;
}

void	free_rule(void *vrule)
{
	t_rule	*rule;

	rule = vrule;
	free(rule->pattern);
	rule->pattern = NULL;
	free(rule->action);
	rule->action = NULL;
	tree_free(rule->tree);
	rule->tree = NULL;
}

void	lex_file_free(t_lex_file *lf)
{
	strbuf_free(&lf->decl_code);
	vec_free(&lf->rules, free_rule);
	lf->user_code = NULL;
}

static const char	*or_null(const char *s)
{
	if (!s)
		return ("(null)");
	return (s);
}

void	lex_file_dump(t_lex_file *lf)
{
	size_t	i;
	t_rule	*rule;

	printf("-- decl_code --\n");
	printf("%s\n\n", or_null(lf->decl_code.data));
	printf("-- rules (%zu) --\n", lf->rules.len);
	i = 0;
	while (i < lf->rules.len)
	{
		rule = vec_get(&lf->rules, i);
		printf("[%zu] line %zu\n", rule->id, rule->line);
		printf("  pattern: \"%s\"\n", or_null(rule->pattern));
		printf("  action : \"%s\"\n", or_null(rule->action));
		i++;
	}
	printf("\n-- user_code --\n");
	printf("%s\n", or_null(lf->user_code));
}
