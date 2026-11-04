%option yylineno
%%
[a-z]	printf("<%s:%d>", yytext, yylineno);
\n	ECHO;
