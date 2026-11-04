%%
if	{ printf("<KW>"); REJECT; }
[a-z]+	printf("<ID:%s>", yytext);
