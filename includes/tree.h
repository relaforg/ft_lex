#ifndef TREE_H
# define TREE_H

typedef enum
{
	N_CHAR,
	N_CAT,
	N_ALT,
	N_STAR,
	N_PLUS
} t_node_type;

typedef struct s_node {
	t_node_type			type;
	union
	{
		unsigned char	c;
	} u;
	struct s_node		*left;
	struct s_node		*right;
}	t_node;

void	tree_print(const t_node *root);
t_node	*node_char(unsigned char c);
t_node	*node_new(t_node_type type, t_node *left, t_node *right);
void	tree_free(t_node *root);

#endif
