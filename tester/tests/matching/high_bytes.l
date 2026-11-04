%%
[a-z]	printf("<%s>", yytext);
.	printf("<%02x>", (unsigned char)yytext[0]);
