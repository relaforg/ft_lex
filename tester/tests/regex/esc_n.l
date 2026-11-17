%%
a\nb	printf("<%s>", yytext);
%%

int	yywrap(void)
{
	return (1);
}

int	main(void)
{
	yylex();
	return (0);
}
