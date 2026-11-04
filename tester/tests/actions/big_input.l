%{
static long words = 0, nums = 0, lines = 0;
%}
%%
[a-z]+	words++;
[0-9]+	nums++;
\n	lines++;
.	;
%%
int main(void) { yylex(); printf("%ld %ld %ld\n", words, nums, lines); return 0; }
