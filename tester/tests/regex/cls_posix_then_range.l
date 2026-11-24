%%
[[:digit:]a-c]	printf("%02x\n", (unsigned char)yytext[0]);
.|\n	;
