%{
/*
 * 03 - Mots-cles et operateurs
 * Teste : priorite a la premiere regle quand deux matchs ont la meme
 *         longueur ("if" -> MOT_CLE, pas IDENT), match le plus long
 *         ("ifx" -> IDENT, "<=" -> un seul token), chaines "...", yyleng.
 * Essai : echo "if ifx while <= < = return returned" | ./scan
 * Attendu : MOT_CLE(if) IDENT(ifx) MOT_CLE(while) OP(<=) OP(<) OP(=)
 *           MOT_CLE(return) IDENT(returned)
 */
#include <stdio.h>
%}

%%
"if"|"while"|"return"	printf("MOT_CLE(%s)\n", yytext);
[a-z]+			printf("IDENT(%s) longueur=%d\n", yytext, (int)yyleng);
"<="			printf("OP(<=)\n");
"<"			printf("OP(<)\n");
"="			printf("OP(=)\n");
[ \t\n]+		;
.			printf("INCONNU(%s)\n", yytext);
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
