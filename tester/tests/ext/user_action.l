%{
#define YY_USER_ACTION printf("{%d}", yyleng);
%}
%%
[a-z]+	;
