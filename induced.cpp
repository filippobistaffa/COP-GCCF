#include "copgccf.h"

#ifdef MINWIDTH
#define METRIC degree
#endif

#ifdef MININDUCEDWIDTH
#define METRIC degree
#define CONNECT
#endif

#ifdef MINFILL
#define METRIC fill
#define CONNECT
#endif

__attribute__((always_inline)) inline
dim degree(dim i, const chunk *adj, const chunk *mark) {

	chunk tmp[CN];
	MASKAND(adj + i * CN, mark, tmp, CN);
	return MASKPOPCNT(tmp, CN);
}

__attribute__((always_inline)) inline
dim fill(dim i, const chunk *adj, const chunk *mark) {

	dim fillcount = 0;
	chunk tmp[CN], tmp1[CN];
	MASKAND(adj + i * CN, mark, tmp, CN);
	const dim popc = MASKPOPCNT(tmp, CN);

	for (dim j = 0, k = MASKFFS(tmp, CN); j < popc; j++, k = MASKCLEARANDFFS(tmp, k, CN)) {
		memcpy(tmp1, tmp, sizeof(chunk) * CN);
		CLEAR(tmp1, k);
		const dim popc1 = MASKPOPCNT(tmp1, CN);
		for (dim h = 0, l = MASKFFS(tmp1, CN); h < popc1; h++, l = MASKCLEARANDFFS(tmp1, l, CN)) fillcount++;
	}

	return fillcount;
}

__attribute__((always_inline)) inline
void connectneighbours(dim i, chunk *adj, const chunk *mark) {

	chunk tmp[CN], tmp1[CN];
	MASKAND(adj + i * CN, mark, tmp, CN);
	const dim popc = MASKPOPCNT(tmp, CN);

	for (dim j = 0, k = MASKFFS(tmp, CN); j < popc; j++, k = MASKCLEARANDFFS(tmp, k, CN)) {
		memcpy(tmp1, tmp, sizeof(chunk) * CN);
		CLEAR(tmp1, k);
		const dim popc1 = MASKPOPCNT(tmp1, CN);
		for (dim h = 0, l = MASKFFS(tmp1, CN); h < popc1; h++, l = MASKCLEARANDFFS(tmp1, l, CN)) EDGE(adj, CN, k, l);
	}
}

dim *greedyorder(chunk *adj) {

	dim *order = (dim *)malloc(sizeof(dim) * N);
	chunk mark[CN], zero[CN], tmp[CN];
	ONES(mark, N, CN);
	memset(zero, 0, sizeof(chunk) * CN);
	dim i = N - 1;

	for (dim j = 0; j < N; j++)
		if (!memcmp(zero, adj + CN * j, sizeof(chunk) * CN)) {
			order[i--] = j;
			CLEAR(mark, j);
			//printf("Clearing %u\n", j);
		}

	for (int j = i; j >= 0; j--) {

		memcpy(tmp, mark, sizeof(chunk) * CN);
		const dim popc = MASKPOPCNT(tmp, CN);
		dim minnode, minmetric = MAXDIM;

		for (dim k = 0, h = MASKFFS(tmp, CN); k < popc; k++, h = MASKCLEARANDFFS(tmp, h, CN)) {
			dim metric = METRIC(h, adj, mark);
			//printf("Metric(%u) = %u\n", h, metric);
			if (metric <= minmetric) {
				minmetric = metric;
				minnode = h;
			}
		}

		//printf("order[%u] = %u\n", i, minnode);
		order[i--] = minnode;
		#ifdef CONNECT
		connectneighbours(minnode, adj, mark);
		#endif
		CLEAR(mark, minnode);
	}

	return order;
}

dim *noorder() {

	dim *order = (dim *)malloc(sizeof(dim) * N);
	for (dim i = 0; i < N; i++) order[i] = i;
	return order;
}

dim inducedwidth(const chunk *adj, const dim *order) {

	dim w[N];
	memset(w, 0, sizeof(dim) * N);
	const dim *a = order + N - 1, *b;
	dim na = N - 1, nb;

	do {
		b = a - 1;
		nb = na;
		do {
			if (GET(adj + *a * CN, *b)) w[*a]++;
			b--;
		} while (--nb);
		a--;
	} while (--na);

	nb = 0;
	for (na = 0; na < N; na++) nb = MAX(nb, w[na]);
	return nb;
}
