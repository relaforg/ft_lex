%%
[\x00-\x05]	printf("%02x\n", (unsigned char)yytext[0]);
.|\n	;
