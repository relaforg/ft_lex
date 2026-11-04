%x A
%%
a	{ printf("%d", YY_START == INITIAL); BEGIN A; }
<A>b	{ printf("%d", YY_START == A); BEGIN INITIAL; }
