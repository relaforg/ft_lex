%{
/*
 * 05 - Contexte
 * Teste : start condition exclusive (%x) et BEGIN, ancrage ^ et $,
 *         contexte de queue r/x.
 * Essai : ./scan < entree_05.txt
 * Attendu :
 *   TITRE(# titre)
 *   MOT(x) NOMBRE(3) MOT(y)       <- le commentaire est ignore
 *   FIN_DE_LIGNE(fin)
 *   AB_AVANT_CHIFFRE(ab) NOMBRE(12) MOT(abc)
 */
#include <stdio.h>
%}

%x COMMENT

%%
"/*"			BEGIN COMMENT;
<COMMENT>"*/"		BEGIN INITIAL;
<COMMENT>.|\n		;
^"#".*			printf("TITRE(%s)\n", yytext);
fin$			printf("FIN_DE_LIGNE(%s)\n", yytext);
ab/[0-9]		printf("AB_AVANT_CHIFFRE(%s)\n", yytext);
[a-z]+			printf("MOT(%s)\n", yytext);
[0-9]+			printf("NOMBRE(%s)\n", yytext);
[ \t\n=]+		;
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
