#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void exec_action(unsigned int action, char *yytext, size_t yyleng) {
	switch (action) {
		case 1: printf("NOMBRE(%s)\n", yytext); break;
		case 2: /*printf("MOT(%s)\n", yytext);*/ break;
		case 3: break;
	}
}


void yylex(char *str) {
	if (!str)
		return;

	int transitions[4][4] = {
		{-1, 1, 2, 3},
		{-1, 1, -1, -1},
		{-1, -1, 2, -1},
		{-1, -1, -1, 3}
	};
	unsigned int actions[4] = {0, 1, 2, 3};
	unsigned int yy_ec[256] = {
/*   0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0,   /* 10 = '\n' */
/*  16 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*  32 */ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /* 32 = ' ' */
/*  48 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,   /* '0'..'9' */
/*  64 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /* majuscules : classe 0 */
/*  80 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*  96 */ 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,   /* 'a'..'o' */
/* 112 */ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,   /* 'p'..'z' */
/* 128 à 255 : tout à 0 */
};

	unsigned int i = 0;
	while (str[i]) {
		int start = i;
		int state = 0;
		unsigned int yy_last_accepting_state = 0;
		unsigned int yy_accepted_position = start;

		unsigned int j = i;
		while (str[j]) {
			state = transitions[state][yy_ec[(unsigned char) str[j]]];
			if (state < 0) {
				break;
			}
			if (actions[state] != 0) {
				yy_last_accepting_state = actions[state];
				yy_accepted_position = j;
			}
			j++;
		}

		if(yy_last_accepting_state == 0)
			printf("%c\n", str[i]);
		else {
			i = yy_accepted_position;	
			size_t yyleng = i - start + 1;
			char *yytext = strndup(str + start, yyleng);
			if (!yytext)
				continue;

			exec_action(yy_last_accepting_state, yytext, yyleng);

			free(yytext);
		}
		i++;
	}
}

char *read_stdin(void) {
	size_t cap = 4096;
	size_t len = 0;
	char *buf = malloc(cap);
	if (!buf)
		return NULL;

	size_t n;
	while ((n = fread(buf + len, 1, cap - len - 1, stdin)) > 0) {
		len += n;
		if (len + 1 == cap) {
			cap *= 2;
			char *tmp = realloc(buf, cap);
			if (!tmp) {
				free(buf);
				return NULL;
			}
			buf = tmp;
		}
	}
	buf[len] = '\0';
	return buf;
}

int main(void) {
	char *input = read_stdin();
	if (!input)
		return (1);
	yylex(input);
	free(input);
	return (0);
}
