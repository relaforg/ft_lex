%{
#include <string.h>
%}
%%
abc	{
		char buf[8];
		int i;
		strcpy(buf, yytext);
		for (i = yyleng - 1; i >= 0; i--)
			unput(buf[i] == 'b' ? 'B' : buf[i]);
	}
aBc	printf("<OK>");
