%%
x	{ int i; for (i = 0; i < 5000; i++) unput('y'); }
y+	printf("<%d>", yyleng);
