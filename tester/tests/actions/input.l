%%
"/*"	{
		int c, prev = 0;
		while ((c = input()) != 0) {
			if (prev == '*' && c == '/')
				break;
			prev = c;
		}
		printf("<C>");
	}
[a-z]	printf("<%s>", yytext);
