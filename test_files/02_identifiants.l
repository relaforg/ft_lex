%{
/*
 * 02 - Identifiants
 * Teste : definitions de noms et substitution {NOM}, parentheses,
 *         union |, etoile *, plusieurs regles en concurrence.
 * Essai : echo "x1 _tmp 42abc var_2 @" | ./scan
 * Attendu : IDENT(x1) IDENT(_tmp) NOMBRE(42) IDENT(abc) IDENT(var_2) INCONNU(@)
 */
#include <stdio.h>
%}

LETTRE		[a-zA-Z_]
CHIFFRE		[0-9]

%%
{LETTRE}({LETTRE}|{CHIFFRE})*	printf("IDENT(%s)\n", yytext);
{CHIFFRE}+			printf("NOMBRE(%s)\n", yytext);
[ \t\n]+			;
.				printf("INCONNU(%s)\n", yytext);
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
