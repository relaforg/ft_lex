/*
 * failalloc : injection d'echecs d'allocation pour ft_lex.
 *
 * Lie avec -Wl,--wrap=malloc,--wrap=calloc,... : chaque appel a malloc() fait
 * DEPUIS LE CODE DE FT_LEX passe par __wrap_malloc(), qui compte les appels et
 * peut renvoyer NULL au N-ieme. __real_malloc() est le vrai malloc (celui
 * d'AddressSanitizer quand le binaire est compile avec -fsanitize=address).
 *
 * Variables d'environnement :
 *   FAILALLOC_AT=N           fait echouer le N-ieme appel (1-based). 0/absent : aucun echec
 *   FAILALLOC_MODE=once|from once (defaut) : seul le N-ieme echoue
 *                            from : le N-ieme ET tous les suivants echouent
 *   FAILALLOC_COUNT_FILE=f   a la sortie, ecrit dans f le nombre total d'appels
 *   FAILALLOC_TRACE=1        affiche sur stderr la pile de l'appel qu'on fait echouer
 *                            (necessite un binaire compile avec -fsanitize=address)
 *
 * Les allocations faites a l'interieur de la libc (printf, fopen...) ne sont
 * pas comptees : seuls les appels ecrits dans les sources de ft_lex le sont.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void	*__real_malloc(size_t size);
void	*__real_calloc(size_t nmemb, size_t size);
void	*__real_realloc(void *ptr, size_t size);
char	*__real_strdup(const char *s);
char	*__real_strndup(const char *s, size_t n);
FILE	*__real_fopen(const char *path, const char *mode);

/* Fourni par le runtime ASan ; reste NULL (symbole faible) dans un binaire sans ASan */
void	__sanitizer_print_stack_trace(void) __attribute__((weak));

static unsigned long	g_count;
static unsigned long	g_fail_at;
static int				g_from;
static int				g_trace;
static int				g_ready;

static void	fa_init(void)
{
	const char	*at;
	const char	*mode;
	const char	*trace;

	if (g_ready)
		return ;
	g_ready = 1;
	at = getenv("FAILALLOC_AT");
	mode = getenv("FAILALLOC_MODE");
	trace = getenv("FAILALLOC_TRACE");
	g_fail_at = at ? strtoul(at, NULL, 10) : 0;
	g_from = mode && strcmp(mode, "from") == 0;
	g_trace = trace && strcmp(trace, "1") == 0;
}

/* Renvoie 1 si cet appel (de la fonction `fn`) doit echouer */
static int	fa_should_fail(const char *fn)
{
	fa_init();
	g_count++;
	if (g_fail_at == 0)
		return (0);
	if (g_count == g_fail_at || (g_from && g_count > g_fail_at))
	{
		if (g_count == g_fail_at && g_trace)
		{
			fprintf(stderr, "failalloc: echec force de l'appel #%lu (%s)\n",
				g_count, fn);
			if (__sanitizer_print_stack_trace)
				__sanitizer_print_stack_trace();
			fprintf(stderr, "failalloc: fin\n");
		}
		errno = ENOMEM;
		return (1);
	}
	return (0);
}

__attribute__((destructor))
static void	fa_report(void)
{
	const char	*path;
	FILE		*f;

	path = getenv("FAILALLOC_COUNT_FILE");
	if (!path)
		return ;
	f = __real_fopen(path, "w");
	if (!f)
		return ;
	fprintf(f, "%lu\n", g_count);
	fclose(f);
}

void	*__wrap_malloc(size_t size)
{
	if (fa_should_fail(__func__ + 7))
		return (NULL);
	return (__real_malloc(size));
}

void	*__wrap_calloc(size_t nmemb, size_t size)
{
	if (fa_should_fail(__func__ + 7))
		return (NULL);
	return (__real_calloc(nmemb, size));
}

/* Un realloc qui echoue renvoie NULL et laisse `ptr` intact (C99 7.20.3.4) */
void	*__wrap_realloc(void *ptr, size_t size)
{
	if (fa_should_fail(__func__ + 7))
		return (NULL);
	return (__real_realloc(ptr, size));
}

char	*__wrap_strdup(const char *s)
{
	if (fa_should_fail(__func__ + 7))
		return (NULL);
	return (__real_strdup(s));
}

char	*__wrap_strndup(const char *s, size_t n)
{
	if (fa_should_fail(__func__ + 7))
		return (NULL);
	return (__real_strndup(s, n));
}

/* fopen alloue (le FILE et son tampon) : il peut echouer avec ENOMEM */
FILE	*__wrap_fopen(const char *path, const char *mode)
{
	if (fa_should_fail(__func__ + 7))
		return (NULL);
	return (__real_fopen(path, mode));
}
