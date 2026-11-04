%%
int	printf("<INT>");
if	printf("<IF>");
[a-z][a-z0-9]*	printf("<ID:%s>", yytext);
[ \n]	ECHO;
