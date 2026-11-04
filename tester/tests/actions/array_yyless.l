%array
%%
foobar	{ printf("<%s>", yytext); yyless(3); }
bar	printf("<B:%s>", yytext);
