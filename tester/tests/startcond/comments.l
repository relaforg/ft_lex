%x COMMENT
%%
"/*"	BEGIN COMMENT;
<COMMENT>"*/"	BEGIN INITIAL;
<COMMENT>.|\n	;
[a-z]	printf("<%s>", yytext);
