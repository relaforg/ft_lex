%x EX
%%
"["	BEGIN EX; printf("<open>");
<EX>"]"	BEGIN INITIAL; printf("<close>");
<EX>a	printf("<EX-a>");
a	printf("<a>");
b	printf("<b>");
