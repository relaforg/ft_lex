%%
[a-z]	{ printf("<%s:%d>", yytext, yyleng); yymore(); }
\n	{ printf("<NL:%d>", yyleng); }
