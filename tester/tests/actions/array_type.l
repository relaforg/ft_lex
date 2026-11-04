%array
%%
[a-z]+	printf("<%d>", (int)(sizeof(yytext) > sizeof(char *)));
%%
extern char yytext[];
