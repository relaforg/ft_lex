%option noyywrap
%%
[a-z]+	printf("<%s>", yytext);
%%
int main(void) { yylex(); return 0; }
