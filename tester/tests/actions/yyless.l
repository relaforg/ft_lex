%%
foobar	{ printf("<%s>", yytext); yyless(3); }
bar	printf("<B:%s>", yytext);
[a-z]+	printf("<W:%s>", yytext);
