%array
%%
ab	{ printf("<%s>", yytext); unput('c'); printf("<%s>", yytext); }
c	printf("<C>");
