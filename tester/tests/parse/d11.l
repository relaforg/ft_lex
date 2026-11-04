%{
int v1 = 1;
%}
D [0-9]
%{
int v2 = 2;
%}
%%
ab printf("%d", v1 + v2);
