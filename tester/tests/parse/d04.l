%{
int block_var = 42;
%} junk
%%
ab printf("%d", block_var);
