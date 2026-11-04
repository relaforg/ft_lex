%%
[a-z]+	printf("<%s>", yytext);
<<EOF>>	{ printf("<EOF>"); yyterminate(); }
