#include "copgccf.h"

#define ALLOCVAR(VAR) var *VAR = (var *)calloc(1, sizeof(var))

dim apt(dim j, const dim *pt, dim *ares) {

	dim ret = pt[j * N];
	memcpy(ares, pt + j * N + 1, sizeof(dim) * ret);

	for (dim k = 0; k < pt[j * N]; k++)
		ret += apt(pt[j * N + k + 1], pt, ares + ret);

	return ret;
}

__attribute__((always_inline)) inline
const var *sp(const var *s, const var *ires, const vector<var> &vars) {

	const var *ret = NULL;
	dim maxcar = 0;
	ALLOCVAR(dres);
	ALLOCVAR(isres);

	for (vector<var>::const_iterator p = vars.begin(); p != vars.end(); ++p) {
		//printc(ITOP(p)->c, 0);
		differencesorted(ITOP(p)->c + 1, *(ITOP(p)->c), ires->c + 1, *(ires->c), dres->c + 1, dres->c);
		//puts("difference");
		//printc(dres->c, 0);
		intersectionsorted(ITOP(p)->c + 1, *(ITOP(p)->c), s->c + 1, *(s->c), isres->c + 1, isres->c);
		//puts("intersection");
		//printc(isres->c, 0);
		if (!*(dres->c) && *(isres->c) > maxcar) {
			maxcar = *(isres->c);
			ret = ITOP(p);
		}
	}

	free(isres);
	free(dres);
	return ret;
}

bool coalreq(vector<var> &cr, const var *s, dim j, const dim *pt, const vector<vector<var>> &vars) {

	bool add = false;
	ALLOCVAR(ares);
	ares->c[1] = j;
	*(ares->c) = 1 + apt(j, pt, ares->c + 2);
	QSORT(dim, ares->c + 1, *(ares->c), LT);
	//puts("ares");
	//printc(ares->c, 0);

	ALLOCVAR(ires);
	intersectionsorted(s->c + 1, *(s->c), ares->c + 1, *(ares->c), ires->c + 1, ires->c);
	//puts("ires");
	//printc(ires->c, 0);

	if (*(ires->c)) {

		ALLOCVAR(dres);
		differencesorted(s->c + 1, *(s->c), ares->c + 1, *(ares->c), dres->c + 1, dres->c);
		//puts("s");
		//printc(s->c, 0);
		//printf("j = %u\n", j);
		//puts("ares");
		//printc(ares->c, 0);
		//puts("dres");
		//printc(dres->c, 0);
		if (*(dres->c)) add = true; //printf(RED("ADD\n"));
		const var *sres = sp(s, ires, vars[j]);
		if (sres) cr.push_back(*sres);

		if (sres) {
			differencesorted(s->c + 1, *(s->c), sres->c + 1, *(sres->c), dres->c + 1, dres->c);
			//puts("sres");
			//printc(sres->c, 0);
		}
		else memcpy(dres, s, sizeof(var));
		//puts("dres");
		//printc(dres->c, 0);
		//ALLOCVAR(ures);

		ALLOCVAR(tmp);

		for (dim k = 0; k < pt[j * N]; k++) {
			differencesorted(dres->c + 1, *(dres->c), &j, 1, tmp->c + 1, tmp->c);
			add |= coalreq(cr, tmp, pt[j * N + k + 1], pt, vars);
		}

		free(tmp);
		free(dres);
	}

	free(ares);
	free(ires);
	return add;
}

void req(vector<vector<dim>> &r, const dim *pt, const vector<vector<var>> &vars, const var *hash) {

	for (dim i = 0; i < N; i++) {
		//printf("Agent #%u\n", i);
		for (vector<var>::const_iterator p = vars[i].begin(); p != vars[i].end(); ++p) {
			//printf("#%u = ", ITOP(p)->i);
			//printc(ITOP(p)->c, ITOP(p)->v);
			ALLOCVAR(tmp);
			for (dim j = 0; j < pt[i * N]; j++) {
				differencesorted(ITOP(p)->c + 1, *(ITOP(p)->c), &i, 1, tmp->c + 1, tmp->c);
				vector<var> cr;
				bool add = coalreq(cr, tmp, pt[i * N + j + 1], pt, vars) || cr.size() > 1;
				for (vector<var>::iterator q = cr.begin(); q != cr.end(); ++q) {
					if (add) {
						memset(tmp->c + 1, 0, sizeof(dim) * K);
						unionsorted(ITOP(q)->c + 1, *(ITOP(q)->c), &i, 1, tmp->c + 1, tmp->c);
						//printf("reqadd = ");
						//printc(tmp->c, 0);
					}
					//else {
					//	printf("req = ");
					//	printc(ITOP(q)->c, ITOP(q)->v);
					//}
					var *qh;
					HASH_FIND(hh, hash, (add ? tmp : ITOP(q))->c, sizeof(dim) * (K + 1), qh);
					//if (!qh) { puts(RED("ERROR")); exit(1); } else printf("id = %u\n", qh->i);
					r[qh->i].push_back(ITOP(p)->i);
				}
			}
			free(tmp);
		}
	}
}
