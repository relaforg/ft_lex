%%
ab	{ yymore(); }
abcd	{ yyless(3); printf("<%s>", yytext); }
[a-z]	printf("<c:%s>", yytext);
