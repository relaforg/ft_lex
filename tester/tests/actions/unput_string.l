%%
x	{ const char *s = "hello"; int i = 5; while (i > 0) unput(s[--i]); }
hello	printf("<HELLO>");
