%%
[a-z]+	printf("<%s>", yytext);
%%
static int n = 0;
int yywrap(void)
{
	if (n++ == 0) {
		yyin = fmemopen("second\n", 7, "r");
		return 0;
	}
	return 1;
}
int main(void) { yylex(); printf("<end:%d>\n", n); return 0; }
