%{ junk
int block_var = 42;
%}
%%
ab printf("%d", block_var);
