%x A B
%{
static char cur = 'I';
%}
%%
1	BEGIN A; cur = 'A';
2	BEGIN B; cur = 'B';
3	BEGIN INITIAL; cur = 'I';
<A,B>x	printf("<AB:%c>", cur);
x	printf("<I>");
<A,B>\n	ECHO;
