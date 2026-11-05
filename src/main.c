#include "lex_file.h"
#include "parse.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ_SIZE 1024

char	*read_fd(FILE *file)
{
	size_t	capacity;
	char	*buf;
	size_t	read_size;
	char	tmp[READ_SIZE];

	capacity = READ_SIZE;
	buf = calloc(1, sizeof(*buf));
	if (!buf)
		return (NULL);
	while (1)
	{
		read_size = fread(tmp, sizeof(*tmp), READ_SIZE, file);
		if (!read_size)
			break ;
		capacity += READ_SIZE;
		buf = realloc(buf, capacity);
		if (!buf)
			return (NULL);
		buf = strncat(buf, tmp, read_size);
	}
	return (buf);
}

int	main(int argc, char **argv)
{
	FILE		*file;
	char		*buf;
	t_lex_file	lf;

	argc--;
	argv++;
	if (argc <= 0)
	{
		file = stdin;
		if (!file)
		{
			dprintf(2, "Error: cannot open stdin\n");
			return (-1);
		}
	}
	else
	{
		file = fopen(argv[0], "r");
		if (!file)
		{
			dprintf(2, "Error: cannot open %s\n", argv[0]);
			return (-1);
		}
	}
	buf = read_fd(file);
	if (!buf)
	{
		dprintf(2, "Error: cannot read\n");
		return (-1);
	}
	if (parse(buf, &lf))
	{
		dprintf(2, "Error: parsing error\n");
		return (-1);
	}
	fclose(file);
	free(buf);
	file = fopen("lex.yy.c", "w");
	fclose(file);
	lex_file_free(&lf);
	return (0);
}
