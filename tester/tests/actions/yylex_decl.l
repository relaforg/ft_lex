%{
int yylex(void);
%}
%%
[a-z]+	printf("<%s>", yytext);
