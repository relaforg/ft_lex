%option stack
%x P
%%
"("	{ yy_push_state(P); printf("<push>"); }
<P>")"	{ yy_pop_state(); printf("<pop>"); }
<P>[a-z]	printf("<p:%s>", yytext);
[a-z]	printf("<%s>", yytext);
