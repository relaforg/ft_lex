%%
[a-z]+	{ char b[32]; snprintf(b, sizeof b, "<%s>", yytext); fputs(b, stdout); }
