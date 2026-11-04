%x Q
%%
"!"	BEGIN Q;
[a-z]	printf("<%s>", yytext);
