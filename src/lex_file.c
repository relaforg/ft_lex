#include "lex_file.h"
#include "strbuf.h"
#include "vec.h"

void	lex_file_init(t_lex_file *lf)
{
	strbuf_init(&lf->decl_code);
	vec_init(&lf->rules, sizeof(t_rule));
	lf->user_code = NULL;
}

void	lex_file_free(t_lex_file *lf)
{
	strbuf_free(&lf->decl_code);
	// Il faudra probablement free les char * dans les rules
	vec_free(&lf->rules, NULL);
	lf->user_code = NULL;
}
