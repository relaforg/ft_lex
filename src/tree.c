#include "tree.h"
#include <stdlib.h>

t_node	*node_char(unsigned char c)
{
	t_node	*new;

	new = malloc(sizeof(t_node));
	if (!new)
		return (NULL);
	new->left = NULL;
	new->right = NULL;
	new->type = N_CHAR;
	new->u.c = c;
	return (new);
}

t_node	*node_new(t_node_type type, t_node *left, t_node *right)
{
	t_node	*new;

	new = malloc(sizeof(t_node));
	if (!new)
		return (NULL);
	new->left = left;
	if (type == N_STAR)
		right = NULL;
	new->right = right;
	new->type = type;
	return (new);
}
void	tree_free(t_node *root)
{
	if (!root)
		return ;
	tree_free(root->left);
	tree_free(root->right);
	free(root);
}
