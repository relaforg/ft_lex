%{
/*
 * 01 - Nombres
 * Teste : classe [0-9], operateur +, match le plus long,
 *         regle "." qui absorbe tout le reste.
 * Essai : echo "abc 42 7x 2026" | ./scan
 * Attendu : NOMBRE(42) NOMBRE(7) NOMBRE(2026)
 */
#include <stdio.h>
%}

%%
[0-9]+		printf("NOMBRE(%s)\n", yytext);
\n		;
.		;
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
