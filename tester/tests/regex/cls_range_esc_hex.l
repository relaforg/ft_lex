%%
[\x01-\x1f]	printf("%02x\n", (unsigned char)yytext[0]);
.|\n	;
