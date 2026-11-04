%%
if	printf("<KW>");
[a-z]+	printf("<ID:%s>", yytext);
