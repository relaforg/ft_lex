#ifndef BITSET_H
# define BITSET_H

# include <stddef.h>
# include <stdint.h>

size_t		bitset_nwords(size_t nbits);
uint64_t	*bitset_new(size_t nwords);
void		bitset_set(uint64_t *set, size_t i);
int			bitset_test(const uint64_t *set, size_t i);
int			bitset_eq(const uint64_t *a, const uint64_t *b, size_t nwords);
int			bitset_is_empty(const uint64_t *set, size_t nwords);

#endif
