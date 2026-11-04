%%
a	return 'a';
\n	return '\n';
%%
int main(void) { int t, n = 0; while ((t = yylex())) n++; printf("<%d>\n", n); return 0; }
