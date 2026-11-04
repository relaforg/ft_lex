%%
(ab|cd){1,30}e	printf("<A%d>", yyleng);
[a-e]{5,40}	printf("<B%d>", yyleng);
[ \n]	ECHO;
