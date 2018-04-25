#include "copgccf.h"

void adjacencylist(const chunk *adj, dim *l) {

	chunk tmp[CN];

	for (dim i = 0; i < N; i++) {
		l[i * N] = 0;										// initialise number of neighbours
		memcpy(tmp, adj + i * CN, sizeof(chunk) * CN);
		const dim popc = MASKPOPCNT(tmp, CN);							// number of set bits
		for (dim j = 0, k = MASKFFS(tmp, CN); j < popc; j++, k = MASKCLEARANDFFS(tmp, k, CN))	// cycle through set bits
			l[i * N + (l[i * N]++) + 1] = k;						// add neighbour
	}
}

#define ORDERGT(I, J) (pos[*I] > pos[*J])								// comparison function based on order

dim pseudotree(const chunk *adj, const dim *order, dim *pt) {

	dim *l = (dim *)malloc(sizeof(dim) * N * N);
	adjacencylist(adj, l);										// adjacency lists of primal graph
	dim pos[N];

	for (dim i = 0; i < N; i++) pos[order[i]] = i;							// pos[i] = j => variable i is in position j in the order
	for (dim i = 0; i < N; i++) QSORT(dim, l + i * N + 1, l[i * N], ORDERGT);			// sort adjacency lists according to order

	dim stack[N * N], parent[N * N];
	chunk mark[CN];
	ONES(mark, N, CN);
	dim root = order[0];
	memset(stack, 0, sizeof(dim) * N);
	memset(parent, 0, sizeof(dim) * N);
	stack[0] = root;
	dim top = 1;

	while (top) {
		dim v = stack[top - 1];								// pop node
		dim p = parent[top - 1];							// pop parent
		top--;
		if (GET(mark, v)) {
			CLEAR(mark, v);								// mark v as visited
			if (v != root) pt[p * N + pt[p * N]++ + 1] = v;				// store children in pseudotree
			memcpy(stack + top, l + v * N + 1, sizeof(dim) * l[v * N]);		// push nodes
			for (dim j = 0; j < l[v * N]; j++) parent[top + j] = v;			// push parents
			top += l[v * N];							// update top
		}
	}

	free(l);
	return root;
}

void descendants(dim i, dim *pt, dim *desc) {

	for (dim j = 0; j < N; j++) QSORT(dim, pt + j * N + 1, pt[j * N], LT);

	if ((desc[i * N] = pt[i * N])) {
		memcpy(desc + i * N + 1, pt + i * N + 1, sizeof(dim) * pt[i * N]);
		dim tmp[N];
		for (dim j = 0; j < pt[i * N]; j++) {
			descendants(pt[i * N + j + 1], pt, desc);
			memcpy(tmp, desc + i * N, sizeof(dim) * (desc[i * N] + 1));
			unionsorted(tmp + 1, tmp[0],
				    desc + pt[i * N + j + 1] * N + 1, desc[pt[i * N + j + 1] * N],
				    desc + i * N + 1, desc + i * N);
		}
	}
}
