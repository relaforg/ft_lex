%{
int state = 1, buf = 2, c = 3, i = 4, n = 5, len = 6, ch = 7, current = 8;
int table = 9, accept = 10, start = 11, pos = 12, size = 13, tmp = 14;
%}
%%
[a-z]+	printf("<%d>", state + buf + c + i + n + len + ch + current + table + accept + start + pos + size + tmp);
