%{
/*
 * 04 - Parsing des actions
 * Teste : actions multi-lignes entre { }, accolades pieges dans une
 *         chaine, un caractere et un commentaire C, action '|'
 *         (meme action que la regle suivante), variables globales.
 * Essai : echo "abc 12 XYZ de 7" | ./scan
 * Attendu : MOT{abc} MOT{de} pour les minuscules, AUTRE(...) pour le
 *           reste, puis le bilan mots=2 autres=3
 */
#include <stdio.h>

int	nb_mots = 0;
int	nb_autres = 0;
%}

%%
[a-z]+		{
			nb_mots++;
			printf("MOT{%s} %c\n", yytext, '}');
		}
[0-9]+		|
[A-Z]+		{
			/* meme action pour les deux regles } piege */
			nb_autres++;
			printf("AUTRE(%s)\n", yytext);
		}
[ \t\n]+	;
%%

int	yywrap(void)
{
	return (1);
}

int	main(void)
{
	yylex();
	printf("mots=%d autres=%d\n", nb_mots, nb_autres);
	return (0);
}
