%%
[a-z]+	printf("<%s>", yytext);
%%
int main(void)
{
	yyin = fmemopen("from memory\n", 12, "r");
	yylex();
	return 0;
}
