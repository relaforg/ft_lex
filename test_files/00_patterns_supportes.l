%{
/*
 * 00 - Patterns supportes
 * Teste : uniquement ce que pattern_parse sait lire aujourd'hui :
 *         caracteres simples, concatenation, union |, etoile *,
 *         parentheses (imbriquees), echappements \x.
 *         Pas de ., [], +, ?, {}, ^, $, "..." : chaque regle doit
 *         produire un arbre (aucune "error" dans -- pattern trees --).
 * Essai : printf '%s\n' 'if abb babb 0 101 xxx yz ** () a|b \ o k else' | ./scan
 * Attendu : MOT_CLE(if) ABB(abb) ABB(babb) BINAIRE(0) BINAIRE(101) X_ETOILES(xxx) YZ PUISSANCE PARENS A_PIPE_B BACKSLASH O_ESPACE_K MOT_CLE(else)
 */
#include <stdio.h>
%}

%%
if|else|while		printf("MOT_CLE(%s)", yytext);
(a|b)*abb		printf("ABB(%s)", yytext);
0|1(0|1)*		printf("BINAIRE(%s)", yytext);
xx**		printf("X_ETOILES(%s)", yytext);
((y))z		printf("YZ");
\*\*		printf("PUISSANCE");
\(\)		printf("PARENS");
a\|b		printf("A_PIPE_B");
\\		printf("BACKSLASH");
o\ k		printf("O_ESPACE_K");
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
