#include "copgccf.h"

cost *createcosts(vector<vector<dim>> &r, const vector<vector<var>> &vars) {

	cost *ret = (cost *)malloc(sizeof(cost) * N);

	for (dim i = 0; i < N; i++) {

		dim card = vars[i].size();
		dim l = card;

		for (vector<var>::const_iterator p = vars[i].begin(); p != vars[i].end(); ++p) {
			sort(r[p->i].begin(), r[p->i].end());
			card += r[p->i].size();
		}

		ret[i].vars = (dim *)malloc(sizeof(dim) * card);

		dim j = 0;
		for (vector<var>::const_iterator p = vars[i].begin(); p != vars[i].end(); ++p, j++)
			ret[i].vars[j] = p->i;

		QSORT(dim, ret[i].vars, vars[i].size(), LT);
		dim *tmp = (dim *)malloc(sizeof(dim) * 2 * card);
		ret[i].a = 0;
		dim k;

		for (vector<var>::const_iterator p = vars[i].begin(); p != vars[i].end(); ++p) {
			j = ret[i].a;
			memcpy(tmp, ret[i].vars + l, sizeof(dim) * ret[i].a);
			differencesorted(&(r[p->i])[0], r[p->i].size(), ret[i].vars, l, tmp + card, &k);
			unionsorted(tmp, j, tmp + card, k, ret[i].vars + l, &(ret[i].a));
		}

		j = 0;
		free(tmp);
		ret[i].a += l;
		ret[i].d = UNVALID;
		ret[i].n = ret[i].a;
		ret[i].r = (row *)calloc(ret[i].n, sizeof(row));
		for (dim j = 0; j < ret[i].n; j++) ret[i].r[j].x = (dim *)calloc(ret[i].a, sizeof(dim));
		ret[i].vars = (dim *)realloc(ret[i].vars, sizeof(dim) * ret[i].a);

		for (vector<var>::const_iterator p = vars[i].begin(); p != vars[i].end(); ++p, j++) {
			ret[i].r[j].v = p->v;
			ret[i].r[j].a = ret[i].a;
			ret[i].r[j].x[j] = 1;
			for (vector<dim>::const_iterator q = r[p->i].begin(); q != r[p->i].end(); ++q) {
				const dim dimx = binarysearch(*q, ret[i].vars, l); // search in local vars
				if (dimx < l) {
					//printf("[local] %u is required by %u (%u)\n", p->i, *q, dimx);
					//SET(DATA(f + i, dimx), j);
					ret[i].r[dimx].x[j] = 1;
				}
			}
		}

		j = 0;
		k = l;

		for (vector<var>::const_iterator p = vars[i].begin(); p != vars[i].end(); ++p, j++)
			for (vector<dim>::const_iterator q = r[p->i].begin(); q != r[p->i].end(); ++q) {
				const dim dimx = binarysearch(*q, ret[i].vars + l, ret[i].a - l); // search in non local vars
				if (dimx < ret[i].a - l) {
					//printf("[non local] %u is required by %u\n", p->i, *q);
					memcpy(ret[i].r[k].x, ret[i].r[j].x, sizeof(dim) * ret[i].a);
					//memcpy(DATA(f + i, k), DATA(f + i, j), sizeof(chunk) * ret[i].c);
					ret[i].r[k].x[dimx + l] = 1;
					//SET(DATA(f + i, k), dimx + l);
					k++;
				}
			}

		//printcost(ret + i);
	}

	return ret;
}

int main(int argc, char *argv[]) {

	#ifndef TWITTER
	if (argc != 4) {
		fprintf(stderr, "Usage: copgccf SEED WCSP ORDER\n");
		exit(1);
	}
	#endif

	#ifndef TWITTER
	// pre-allocate the necessary data structures
	chunk *adj = (chunk *)calloc(N * CN, sizeof(chunk));
	init(atoi(argv[1])); // initialise pseudo-random number generator with the first seed
	CREATEGRAPH(adj);
	#else
	init(SEED);
	#endif

	printadj(adj);

	chunk *adjtmp = (chunk *)malloc(sizeof(chunk) * N * CN);
	memcpy(adjtmp, adj, sizeof(chunk) * N * CN);
	dim *order = greedyorder(adjtmp);
	free(adjtmp);

	printbuf(order, N, "order");
	printf("Induced width = %u\n", inducedwidth(adj, order));

	vector<vector<var>> vars(N, vector<var>());
	dim *pt = (dim *)calloc(N * N, sizeof(dim));
	dim root = pseudotree(adj, order, pt);
	dim *desc = (dim *)malloc(sizeof(dim) * N * N);
	descendants(root, pt, desc);
	#ifdef UI
	showgraph(adj);
	showpseudotree(pt, root);
	#endif
	printf("%zu variables\n", COALITIONS(adj, vars, desc));

	for (dim i = 0; i < N; i++) {
		printf("Agent #%u:\n", i);
		for (vector<var>::iterator it = vars[i].begin(); it != vars[i].end(); ++it)
			printc(ITOP(it)->c, ITOP(it)->v);
	}

	dim varid = 0;
	var *hash = NULL;

	for (dim i = 0; i < N; i++)
		for (vector<var>::iterator it = vars[i].begin(); it != vars[i].end(); ++it) {
			ITOP(it)->i = varid++;
			//printf("%3u: ", p->i);
			//printc(p->c, p->v);
			HASH_ADD(hh, hash, c, sizeof(dim) * (K + 1), ITOP(it));
		}

	//printf("root = %u\n", root);

	/*for (dim i = 0; i < N; i++) {
		printf("%u children = ", i);
		for (dim j = 0; j < pt[i * N]; j++)
			printf("%u ", pt[i * N + j + 1]);
		puts("");
	}*/

	/*for (dim i = 0; i < N; i++) {
		printf("%u descendants = ", i);
		for (dim j = 0; j < desc[i * N]; j++)
			printf("%u ", desc[i * N + j + 1]);
		puts("");
	}*/

	vector<vector<dim>> r(HASH_CNT(hh, hash), vector<dim>());
	req(r, pt, vars, hash);

	/*for (dim i = 0; i < HASH_CNT(hh, hash); i++) {
		printf("Variables requiring #%u: ", i);
		for (vector<dim>::iterator it = r[i].begin(); it != r[i].end(); ++it)
			printf("#%u ", *it);
		puts("");
	}*/

	cost *ca = createcosts(r, vars);
	writewcspfile(ca, HASH_COUNT(hash), atoi(argv[1]), argv[2]);
	writeorderfile(vars, pt, argv[3]);

	for (dim i = 0; i < N; i++) {
		for (dim j = 0; j < ca[i].n; j++) free(ca[i].r[j].x);
		free(ca[i].vars);
		free(ca[i].r);
	}

	HASH_CLEAR(hh, hash);
	free(desc);
	free(adj);
	free(pt);
	free(ca);

	return 0;
}
