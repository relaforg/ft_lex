%{
static int he = 0, she = 0;
%}
%%
she	{ she++; REJECT; }
he	{ he++; REJECT; }
.|\n	;
%%
int main(void) { yylex(); printf("he=%d she=%d\n", he, she); return 0; }
