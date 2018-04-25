#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>
typedef uint64_t chunk;
#define BITSPERCHUNK 64
#define ZERO 0ULL
#define ONE 1ULL
#define BITFORMAT "%" WIDTH "lu"

typedef uint32_t dim;
typedef uint32_t value;

#include <limits.h>
#define MAXDIM UINT_MAX
#define MAXVAL UINT_MAX

typedef struct {
        dim a, *x;
        value v;
} row;

typedef struct {
        dim a, n, *vars;
	value d;
        row *r;
} cost;

typedef struct {
	dim i, c[K + 1];
	UT_hash_handle hh;
	value v;
} var;

#endif  /* TYPES_H_ */
