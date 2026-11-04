%{
static int f(void) { return 1; }
static const char *s = "%}";
%}
%%
ab printf("%d%s", f(), s);
