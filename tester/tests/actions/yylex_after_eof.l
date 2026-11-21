%%
a	printf("[a]");
%%
int main(void) { printf("%d", yylex()); printf("%d", yylex()); return 0; }
