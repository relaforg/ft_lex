%x STR
%%
\"	BEGIN STR;
<STR>[a-z]+	printf("<s:%s>", yytext);
<STR><<EOF>>	{ printf("<unterminated>"); yyterminate(); }
