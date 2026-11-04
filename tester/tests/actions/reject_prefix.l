%%
[a-z]+	{ printf("<%s>", yytext); REJECT; }
.|\n	printf("|%s|", yytext);
