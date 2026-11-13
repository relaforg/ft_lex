#include "bitset.h"
#include <stdlib.h>
#include <string.h>

size_t	bitset_nwords(size_t nbits)
{
	// 0 renvoie 0 alors que avec nbits / 64 + 1 0 renvoie 1
	return ((nbits + 63) / 64);
}

uint64_t	*bitset_new(size_t nwords)
{
	return (calloc(nwords, sizeof(uint64_t)));
}

void	bitset_set(uint64_t *set, size_t i)
{
	set[i / 64] |= (uint64_t)1 << (i % 64);
}

int	bitset_test(const uint64_t *set, size_t i)
{
	return ((set[i / 64] >> (i % 64) & 1) == 1);
}

int	bitset_eq(const uint64_t *a, const uint64_t *b, size_t nwords)
{
	return (!memcmp(a, b, nwords * sizeof(uint64_t)));
}

int	bitset_is_empty(const uint64_t *set, size_t nwords)
{
	size_t	i;

	i = 0;
	while (i < nwords)
	{
		if (set[i])
			return (0);
		i++;
	}
	return (1);
}
