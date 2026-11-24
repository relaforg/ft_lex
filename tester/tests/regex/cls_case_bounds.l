%%
[[:upper:]]+	printf("U<%s>", yytext);
[[:lower:]]+	printf("L<%s>", yytext);
