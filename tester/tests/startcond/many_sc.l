%x S0 S1 S2 S3 S4 S5 S6 S7 S8 S9
%{
static int cur = -1;
%}
%%
%{
int st[10];
st[0] = S0; st[1] = S1; st[2] = S2; st[3] = S3; st[4] = S4;
st[5] = S5; st[6] = S6; st[7] = S7; st[8] = S8; st[9] = S9;
%}
<INITIAL,S0,S1,S2,S3,S4,S5,S6,S7,S8,S9>[0-9]	{ printf("<%d>", cur); cur = yytext[0] - '0'; BEGIN st[cur]; }
<INITIAL,S0,S1,S2,S3,S4,S5,S6,S7,S8,S9>\n	ECHO;
