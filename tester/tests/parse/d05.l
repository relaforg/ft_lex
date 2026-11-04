%{
int block_var = 42;
 /* %} */
int other = 1;
%}
%%
ab printf("%d", block_var + other);
