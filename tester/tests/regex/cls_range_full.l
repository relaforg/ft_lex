%%
[\x01-\xff]	printf("%02x\n", (unsigned char)yytext[0]);
.|\n	;
