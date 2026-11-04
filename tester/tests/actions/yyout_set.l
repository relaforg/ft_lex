%%
[a-z]+	ECHO;
%%
int main(void)
{
	yyout = stderr;
	yylex();
	printf("<done>");
	return 0;
}
