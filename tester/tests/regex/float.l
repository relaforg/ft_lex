D	[0-9]
E	[eE][-+]?{D}+
%%
{D}+{E}	|
{D}*"."{D}+({E})?	|
{D}+"."{D}*({E})?	printf("<F:%s>", yytext);
{D}+	printf("<I:%s>", yytext);
