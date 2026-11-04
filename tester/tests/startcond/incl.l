%s IN
%%
"["	BEGIN IN; printf("<open>");
"]"	BEGIN INITIAL; printf("<close>");
<IN>a	printf("<IN-a>");
a	printf("<a>");
b	printf("<b>");
