#include "tree.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Affichage vertical, racine en haut :
 *
 *        |
 *    ┌───┴───┐
 *    ·      'c'
 *  ┌─┴─┐
 * 'a'  *
 *      │
 *     'b'
 *
 * Principe : on dessine dans un "canvas" 2D (une grille de caracteres) puis
 * on l'imprime ligne par ligne. Les cases sont des code points Unicode
 * (uint32_t) et pas des char, car '┌', '─' etc. font plusieurs octets en
 * UTF-8 mais une seule colonne a l'ecran.
 *
 * Ligne 2*d   : les labels des noeuds de profondeur d
 * Ligne 2*d+1 : les connecteurs vers leurs enfants
 */

#define GAP 2
#define LABEL_MAX 8

typedef struct s_canvas
{
	uint32_t	*cells;
	size_t		width;
	size_t		height;
}				t_canvas;

static size_t	node_label(const t_node *node, uint32_t out[LABEL_MAX])
{
	char	buf[LABEL_MAX];
	int		len;
	int		i;

	/* Pas de `default` : avec -Wall, oublier un type de noeud = warning */
	switch (node->type)
	{
	case N_CAT:
		out[0] = 0x00B7; /* '·' */
		return (1);
	case N_ALT:
		out[0] = '|';
		return (1);
	case N_STAR:
		out[0] = '*';
		return (1);
	case N_PLUS:
		out[0] = '+';
		return (1);
	case N_CHAR:
		break ;
	}
	if (node->u.c == '\n')
		len = snprintf(buf, sizeof(buf), "'\\n'");
	else if (node->u.c == '\t')
		len = snprintf(buf, sizeof(buf), "'\\t'");
	else if (node->u.c == '\'' || node->u.c == '\\')
		len = snprintf(buf, sizeof(buf), "'\\%c'", node->u.c);
	else if (isprint(node->u.c))
		len = snprintf(buf, sizeof(buf), "'%c'", node->u.c);
	else
		len = snprintf(buf, sizeof(buf), "'\\x%02x'", node->u.c);
	i = -1;
	while (++i < len)
		out[i] = (unsigned char)buf[i];
	return ((size_t)len);
}

/* Largeur en colonnes du sous-arbre (label compris) */
static size_t	node_width(const t_node *node)
{
	uint32_t	label[LABEL_MAX];
	size_t		len;
	size_t		w;

	len = node_label(node, label);
	w = 0;
	if (node->left && node->right)
		w = node_width(node->left) + GAP + node_width(node->right);
	else if (node->left)
		w = node_width(node->left);
	if (len > w)
		return (len);
	return (w);
}

static size_t	node_depth(const t_node *node)
{
	size_t	l;
	size_t	r;

	if (!node)
		return (0);
	l = node_depth(node->left);
	r = node_depth(node->right);
	if (l > r)
		return (l + 1);
	return (r + 1);
}

static void	put(t_canvas *cv, size_t row, size_t col, uint32_t cp)
{
	cv->cells[row * cv->width + col] = cp;
}

static size_t	draw(const t_node *node, t_canvas *cv, size_t x0, size_t row);

/* Deux enfants : on les pose cote a cote, le parent au milieu */
static size_t	draw_binary(const t_node *node, t_canvas *cv, size_t x0,
		size_t row)
{
	size_t	wl;
	size_t	off;
	size_t	lc;
	size_t	rc;
	size_t	col;

	wl = node_width(node->left);
	off = x0 + (node_width(node) - (wl + GAP + node_width(node->right))) / 2;
	lc = draw(node->left, cv, off, row + 2);
	rc = draw(node->right, cv, off + wl + GAP, row + 2);
	col = lc;
	while (++col < rc)
		put(cv, row + 1, col, 0x2500);       /* '─' */
	put(cv, row + 1, lc, 0x250C);            /* '┌' */
	put(cv, row + 1, rc, 0x2510);            /* '┐' */
	put(cv, row + 1, (lc + rc) / 2, 0x2534); /* '┴' */
	return ((lc + rc) / 2);
}

/* Dessine le sous-arbre dans [x0, x0 + largeur),
	renvoie la colonne du centre */
static size_t	draw(const t_node *node, t_canvas *cv, size_t x0, size_t row)
{
	uint32_t	label[LABEL_MAX];
	size_t		len;
	size_t		w;
	size_t		center;
	size_t		i;

	len = node_label(node, label);
	w = node_width(node);
	if (node->left && node->right)
		center = draw_binary(node, cv, x0, row);
	else if (node->left)
	{
		center = draw(node->left, cv, x0 + (w - node_width(node->left)) / 2, row
				+ 2);
		put(cv, row + 1, center, 0x2502); /* '│' */
	}
	else
		center = x0 + w / 2;
	if (center < x0 + len / 2)
		center = x0 + len / 2;
	i = 0;
	while (i < len)
	{
		put(cv, row, center - len / 2 + i, label[i]);
		i++;
	}
	return (center);
}

static void	put_utf8(uint32_t cp)
{
	if (cp < 0x80)
		putchar((int)cp);
	else if (cp < 0x800)
	{
		putchar((int)(0xC0 | cp >> 6));
		putchar((int)(0x80 | (cp & 0x3F)));
	}
	else
	{
		putchar((int)(0xE0 | cp >> 12));
		putchar((int)(0x80 | (cp >> 6 & 0x3F)));
		putchar((int)(0x80 | (cp & 0x3F)));
	}
}

static void	canvas_print(const t_canvas *cv)
{
	size_t	row;
	size_t	col;
	size_t	end;

	row = 0;
	while (row < cv->height)
	{
		end = cv->width;
		while (end > 0 && cv->cells[row * cv->width + end - 1] == ' ')
			end--;
		col = 0;
		while (col < end)
			put_utf8(cv->cells[row * cv->width + col++]);
		putchar('\n');
		row++;
	}
}

void	tree_print(const t_node *root)
{
	t_canvas	cv;
	size_t		i;

	if (!root)
	{
		printf("(empty)\n");
		return ;
	}
	cv.width = node_width(root);
	cv.height = 2 * node_depth(root) - 1;
	cv.cells = malloc(cv.width * cv.height * sizeof(*cv.cells));
	if (!cv.cells)
	{
		printf("(tree_print: alloc error)\n");
		return ;
	}
	i = 0;
	while (i < cv.width * cv.height)
		cv.cells[i++] = ' ';
	draw(root, &cv, 0, 0);
	canvas_print(&cv);
	free(cv.cells);
}
