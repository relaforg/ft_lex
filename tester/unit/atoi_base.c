/*
** Tests unitaires de ft_atoi_base (src/ft_atoi_base.s).
** Lance par `make unit` (ou `make vunit` sous valgrind) depuis tester/.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	ft_atoi_base(char *str, const char *base);
int	abi_call_atoi_base(char *str, const char *base, int *clobbered);

#define HEX_UP "0123456789ABCDEF"
#define HEX_LO "0123456789abcdef"
#define OCT "01234567"
#define DEC "0123456789"

typedef struct s_case
{
	const char	*str;
	const char	*base;
	int			want;
	const char	*desc;
}	t_case;

static const t_case	g_cases[] = {
	/* conversions simples */
	{"101", "01", 5, "binaire"},
	{"42", DEC, 42, "decimal"},
	{"41", HEX_UP, 0x41, "hexa majuscules"},
	{"ff", HEX_LO, 255, "hexa minuscules avec base minuscule"},
	{"FF", HEX_UP, 255, "hexa majuscules avec base majuscule"},
	{"101", OCT, 65, "octal"},
	{"777", OCT, 511, "octal > 255 : atoi_base ne tronque pas"},
	{"0", DEC, 0, "zero"},
	{"", DEC, 0, "chaine vide"},
	/* arret sur un caractere hors base */
	{"ff", HEX_UP, 0, "minuscules absentes d'une base majuscule"},
	{"7G", HEX_UP, 7, "arret sur G"},
	{"18", OCT, 1, "8 n'est pas octal"},
	{"12 34", DEC, 12, "arret sur l'espace"},
	/* signes */
	{"-42", DEC, -42, "signe -"},
	{"+42", DEC, 42, "signe +"},
	{"-", DEC, 0, "signe - seul"},
	{"+", DEC, 0, "signe + seul"},
	{"--1", DEC, 0, "double signe"},
	/* bases invalides : 0 */
	{"1", "", 0, "base vide"},
	{"0", "0", 0, "base de taille 1"},
	{"1", "011", 0, "base avec doublon"},
	{"1", "0+1", 0, "base avec +"},
	{"1", "0-1", 0, "base avec -"},
	{"1", "0 1", 0, "base avec espace"},
	{"1", "0\t1", 0, "base avec tabulation"},
};

static const char	*g_regs[] = {"rbx", "rbp", "r12", "r13", "r14", "r15", "rsp"};

/* Copie dans un buffer malloc de la taille exacte : valgrind voit alors
** toute lecture au-dela du '\0' (impossible avec un litteral en .rodata). */
static char	*dup_exact(const char *s)
{
	size_t	n;
	char	*p;

	n = strlen(s) + 1;
	p = malloc(n);
	if (p)
		memcpy(p, s, n);
	return (p);
}

static int	run_case(const t_case *c)
{
	char	*str;
	char	*base;
	int		got;
	int		clobbered;
	int		ok;
	int		r;

	str = dup_exact(c->str);
	base = dup_exact(c->base);
	if (!str || !base)
		return (free(str), free(base), 0);
	got = abi_call_atoi_base(str, base, &clobbered);
	ok = (got == c->want && clobbered == 0);
	printf("  %s  %-44s atoi_base(\"%s\", \"%s\")", ok ? "OK" : "KO",
		c->desc, c->str, c->base);
	if (got != c->want)
		printf(" = %d, attendu %d", got, c->want);
	if (clobbered)
	{
		printf(" ; registres callee-saved ecrases :");
		for (r = 0; r < 7; r++)
			if (clobbered & (1 << r))
				printf(" %s", g_regs[r]);
	}
	printf("\n");
	free(str);
	free(base);
	return (ok);
}

int	main(void)
{
	size_t	i;
	size_t	n;
	size_t	ok;

	n = sizeof(g_cases) / sizeof(g_cases[0]);
	ok = 0;
	printf("== unit : ft_atoi_base ==\n");
	for (i = 0; i < n; i++)
		ok += run_case(&g_cases[i]);
	printf("  => %zu OK, %zu KO\n", ok, n - ok);
	return (ok != n);
}
