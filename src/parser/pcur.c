#include "pattern.h"

void	pcur_init(t_pcur *p, const char *s)
{
	p->s = s;
	p->pos = 0;
	p->depth = 0;
	p->err = PAT_OK;
}

unsigned char	pcur_peek(const t_pcur *p)
{
	return (p->s[p->pos]);
}

void	pcur_advance(t_pcur *p)
{
	if (pcur_peek(p))
		p->pos++;
}

int	pcur_match(t_pcur *p, char c)
{
	if (pcur_peek(p) != c)
		return (0);
	pcur_advance(p);
	return (1);
}

t_node	*pcur_fail(t_pcur *p, t_pattern_error err)
{
	p->err = err;
	return (NULL);
}

int	pcur_enter(t_pcur *p)
{
	if (p->depth >= PATTERN_MAX_DEPTH)
	{
		p->err = PAT_ERR_TOO_DEEP;
		return (0);
	}
	p->depth++;
	return (1);
}

void	pcur_leave(t_pcur *p)
{
	p->depth--;
}
