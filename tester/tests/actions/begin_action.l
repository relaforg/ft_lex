%x STR
%%
\"	{ BEGIN STR; printf("<str>"); }
<STR>\"	{ BEGIN INITIAL; printf("</str>"); }
<STR>[^"]+	printf("[%s]", yytext);
