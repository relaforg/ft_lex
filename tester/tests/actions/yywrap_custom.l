%%
[a-z]+	printf("<%s>", yytext);
%%
int yywrap(void) { printf("<wrap>"); return 1; }
int main(void) { yylex(); printf("<end>\n"); return 0; }
