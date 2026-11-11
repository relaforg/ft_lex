#include "pattern.h"
#include "tree.h"

t_pattern_error	pattern_parse(const char *pattern, t_node **out,
		size_t *err_pos)
{
	t_pcur	p;
	t_node	*tree;

	*out = NULL;
	pcur_init(&p, pattern);
	tree = parse_alt(&p);
	if (!tree)
	{
		*err_pos = p.pos;
		return (p.err);
	}
	if (pcur_peek(&p) == ')')
	{
		tree_free(tree);
		*err_pos = p.pos;
		return (PAT_ERR_UNMATCHED_PAREN);
	}
	*out = tree;
	return (PAT_OK);
}

t_node	*parse_alt(t_pcur *p)
{
	t_node	*left;
	t_node	*right;
	t_node	*tmp;

	if (!pcur_peek(p) || pcur_peek(p) == '|' || pcur_peek(p) == ')')
		return (pcur_fail(p, PAT_ERR_EMPTY_ALTERNATIVE));
	left = parse_cat(p);
	if (!left)
		return (NULL);
	while (pcur_peek(p) == '|')
	{
		pcur_advance(p);
		if (!pcur_peek(p) || pcur_peek(p) == '|' || pcur_peek(p) == ')')
		{
			tree_free(left);
			return (pcur_fail(p, PAT_ERR_EMPTY_ALTERNATIVE));
		}
		right = parse_cat(p);
		if (!right)
		{
			tree_free(left);
			return (NULL);
		}
		tmp = node_new(N_ALT, left, right);
		if (!tmp)
		{
			tree_free(left);
			tree_free(right);
			return (pcur_fail(p, PAT_ERR_ALLOC));
		}
		left = tmp;
	}
	return (left);
}

t_node	*parse_cat(t_pcur *p)
{
	t_node	*left;
	t_node	*right;
	t_node	*tmp;

	left = parse_star(p);
	if (!left)
		return (NULL);
	while (pcur_peek(p) && pcur_peek(p) != '|' && pcur_peek(p) != ')')
	{
		right = parse_star(p);
		if (!right)
		{
			tree_free(left);
			return (NULL);
		}
		tmp = node_new(N_CAT, left, right);
		if (!tmp)
		{
			tree_free(left);
			tree_free(right);
			return (pcur_fail(p, PAT_ERR_ALLOC));
		}
		left = tmp;
	}
	return (left);
}

t_node	*parse_star(t_pcur *p)
{
	t_node	*node;
	t_node	*tmp;

	node = parse_atom(p);
	if (!node)
		return (NULL);
	while (pcur_peek(p) == '*')
	{
		tmp = node_new(N_STAR, node, NULL);
		if (!tmp)
		{
			tree_free(node);
			return (pcur_fail(p, PAT_ERR_ALLOC));
		}
		node = tmp;
		pcur_advance(p);
	}
	return (node);
}

t_node	*parse_atom(t_pcur *p)
{
	unsigned char	c;
	t_node			*node;
	size_t			start;
	t_node			*sub;

	c = pcur_peek(p);
	if (c == '\0' || c == '|' || c == ')' || c == '*')
		return (pcur_fail(p, PAT_ERR_MISSING_OPERAND));
	else if (c == '\\' && !p->s[p->pos + 1])
		return (pcur_fail(p, PAT_ERR_TRAILING_BACKSLASH));
	else if (c == '\\')
	{
		pcur_advance(p);
		c = pcur_peek(p);
	}
	else if (c == '[' || c == '.' || c == '+' || c == '?' || c == '{'
		|| c == '^' || c == '$' || c == '"')
		return (pcur_fail(p, PAT_ERR_UNSUPPORTED));
	else if (c == '(')
	{
		start = p->pos;
		if (!pcur_enter(p))
			return (NULL);
		pcur_advance(p);
		sub = parse_alt(p);
		if (!sub)
			return (NULL);
		if (!pcur_match(p, ')'))
		{
			tree_free(sub);
			p->pos = start;
			return (pcur_fail(p, PAT_ERR_UNCLOSED_PAREN));
		}
		pcur_leave(p);
		return (sub);
	}
	pcur_advance(p);
	node = node_char(c);
	if (!node)
		return (pcur_fail(p, PAT_ERR_ALLOC));
	return (node);
}
