%%
[a-z]+	return 1;
[0-9]+	return 2;
.|\n	;
%%
int main(void)
{
	int t;
	while ((t = yylex()) != 0)
		printf("<%d:%s>", t, yytext);
	printf("<end>\n");
	return 0;
}
