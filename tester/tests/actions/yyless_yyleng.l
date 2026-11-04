%%
abcdef	{ yyless(2); printf("<%s:%d>", yytext, yyleng); }
[a-z]+	printf("<R:%s>", yytext);
