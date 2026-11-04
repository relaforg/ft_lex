%%
[a-z]+	printf("<%s>", yytext);
%%
int main(void) { yy_scan_string("hello world"); yylex(); return 0; }
