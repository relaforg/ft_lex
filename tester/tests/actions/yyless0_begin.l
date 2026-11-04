%x VAL
%%
[a-z]+"="	{ yyless(yyleng - 1); printf("<K:%s>", yytext); BEGIN VAL; }
<VAL>"="	;
<VAL>[0-9]+	{ printf("<V:%s>", yytext); BEGIN INITIAL; }
