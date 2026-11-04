/* libl de reference (utilisee par `make golden` et `make selftest`) */
extern int yylex(void);

int main(void)
{
	while (yylex() != 0)
		;
	return 0;
}
