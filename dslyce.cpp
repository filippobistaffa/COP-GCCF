#include "copgccf.h"

typedef int16_t sign;
static size_t bcm[(N + 1) * (N + 1)], pm[N * N];
static const uint8_t T = omp_get_max_threads();

#define P(_s, _i) (pm[(_s) * N + (_i)])
#define B(_n, _m) (bcm[(_n) * (N + 1) + (_m)])
#define coallt(_a, _b) (memcmp(_a, _b, sizeof(coal)) < 0)

__attribute__((always_inline)) inline
void filltables() {

	for (dim i = 0; i <= N; i++) B(i, 0) = B(i, i) = 1ULL;
	for (dim i = 1; i <= N; i++) for (dim j = 1; j < i; j++) B(i, j) = B(i - 1, j - 1) + B(i - 1, j);
	for (dim i = 1; i < N; i++) P(i, 1) = 1ULL;
	for (dim i = 2; i < N; i++) P(1, i) = i;
	for (dim i = 2; i < N; i++) for (dim j = 2; j < N; j++) P(i, j) = P(i - 1, j) + P(i, j - 1);
}

__attribute__((always_inline)) inline
sign twiddle(sign *x, sign *y, sign *z, sign *p) {

	sign i, j = 1, k;
	while (p[j] <= 0) j++;

	if (p[j - 1] == 0) {

		for (i = j - 1; i != 1; i--) p[i] = -1;

		p[j] = 0;
		*x = *z = 0;
		p[1] = 1;
		*y = j - 1;
	}
	else {
		if (j > 1) p[j - 1] = 0;

		do j++;
		while (p[j] > 0);

		k = j - 1;
		i = j;

		while (p[i] == 0) p[i++] = -1;

		if (p[i] == -1) {

			p[i] = p[k];
			*z = p[k] - 1;
			*x = i - 1;
			*y = k - 1;
			p[k] = -1;
		}
		else {
			if (i == p[0]) return 1;
			else {
				p[j] = p[i];
				*z = p[i] - 1;
				p[i] = 0;
				*x = j - 1;
				*y = i - 1;
			}
		}
	}

	return 0;
}

__attribute__((always_inline)) inline
void inittwiddle(sign m, sign n, sign *p) {

	sign i;
	p[0] = n + 1;

	for (i = 1; i != n - m + 1; i++) p[i] = 0;

	while (i != n + 1) {
		p[i] = i + m - n;
		i++;
	}

	p[n + 1] = -2;
	if (m == 0) p[1] = 1;
}

void printc(const dim *c, value v) {

	dim n = *c;
	printf("{ ");
	while (n--) printf("%u ", *(++c));
	std::cout << "} = " << v << std::endl;
}

__attribute__((always_inline)) inline
void removedim(dim x, const dim *l, dim *o) {

	for (dim i = 0; i < N; i++) {
		dim j = binarysearch(x, l + i * N + 1, l[i * N]);
		if (j < l[i * N]) {
			memcpy(o + i * N + 1, l + i * N + 1, sizeof(dim) * j);
			memcpy(o + i * N + j + 1, l + i * N + j + 2, sizeof(dim) * (l[i * N] - j - 1));
			o[i * N] = l[i * N] - 1;
		}
		else memcpy(o + i * N, l + i * N, sizeof(dim) * N);
	}
}

__attribute__((always_inline)) inline
void neighbours(const dim *f, dim m, const dim *l, dim *n) {

	if (m) {
		dim t[N + 1];
		memcpy(n, l + *f * N, sizeof(dim) * (l[*f * N] + 1));
		f++;

		while (--m) {
			unionsorted(n + 1, *n, l + *f * N + 1, l[*f * N], t + 1, t);
			memcpy(n, t, sizeof(dim) * (*t + 1));
			f++;
		}
	}
	else *n = 0;
}

__attribute__((always_inline)) inline
void nbar(const dim *f, dim n, const dim *r, const dim *ruf, const dim *l, dim *nb) {

	dim a[N + 1], b[N + 1];
	neighbours(f, n, l, a);
	dim i = 0;
	while (i < *a && LE(a + i + 1, ruf + 1)) i++;
	memmove(a + 1, a + i + 1, sizeof(dim) * (*a - i));
	*a -= i;
	neighbours(r + 1, *r, l, nb);
	unionsorted(nb + 1, *nb, ruf + 1, *ruf, b + 1, b);
	differencesorted(a + 1, *a, b + 1, *b, nb + 1, nb);
}

template <typename type> __attribute__((always_inline)) inline
type vectorsum(const dim *r, dim n, const type *x) {

	type ret = 0;
	do ret += x[*(r++)];
	while (--n);
	return ret;
}

#define ISANCESTOR(X, Y, D) (binarysearch(Y, (D) + (X) * N + 1, (D)[(X) * N]) < (D)[(X) * N])

__attribute__((always_inline)) inline
dim maxdim(const dim *c, const dim *desc) {

	dim ret = c[1];

	for (dim i = 1; i < c[0]; i++)
		if (ISANCESTOR(c[i + 1], ret, desc))
			ret = c[i + 1];

	return ret;
}

__attribute__((always_inline)) inline
uint8_t coalition(dim *c, vector<vector<var>> &vars, const dim *desc) {

	var t = { 0 };
	memcpy(t.c, c, sizeof(dim) * (c[0] + 1));
	t.v = nextInt(MAXV);
	//#ifdef PRINTINFO
	//printf("Value for coalition ");
	//printc(c, t->v);
	//#endif
	//t.v = 0;
	const dim ma = maxdim(c, desc);
	vars[ma].push_back(t);
	return 1;
}

size_t slyce(dim *r, dim *f, dim m, const dim *l, vector<vector<var>> &vars, const dim *desc) {

	size_t ret = 0;

	if (*r) ret += coalition(r, vars, desc);

	if (*f && m) {

		dim k, *nr = r + K + 1, *nf = f + N + 1, *nfs = nr + *r + 1, fs[N], rt[N];
		memcpy(rt, r + 1, sizeof(dim) * *r);
		sign w, y, z, p[N + 2];

		for (k = 1; k <= MIN(*f, m); k++) {
			*nr = *r + k;
			memcpy(nr + 1, r + 1, sizeof(dim) * *r);
			memcpy(fs, f + *f - k + 1, sizeof(dim) * k);
			memcpy(nfs, fs, sizeof(dim) * k);
			QSORT(dim, nr + 1, *nr, LT);
			nbar(fs, k, r, nr, l, nf);
			ret += slyce(nr, nf, m - k, l, vars, desc);
			inittwiddle(k, *f, p);
			while (!twiddle(&w, &y, &z, p)) {
				fs[z] = f[w + 1];
				memcpy(nr + 1, rt, sizeof(dim) * *r);
				memcpy(nfs, fs, sizeof(dim) * k);
				QSORT(dim, nr + 1, *nr, LT);
				nbar(fs, k, r, nr, l, nf);
				ret += slyce(nr, nf, m - k, l, vars, desc);
			}
		}
	}

	return ret;
}

__attribute__((always_inline)) inline
void li(const dim *f, dim i, dim s, dim *c) {

	dim t = 0, *o = c;
	i = B(*f, s) - i + 1;
	*(o++) = s;

	do {
		dim x = 1;
		while (P(s, x) < (size_t)i - t) x++;
		*(o++) = (*f - s + 1) - x + 1;
		if (P(s, x) == (size_t)i - t) {
			while (s-- - 1) { *o = *(o - 1) + 1; o++; }
			break;
		}
		i -= t;
		t = P(s, x - 1);
	} while (--s);

	o = c + 1;
	s = *c;

	do { *o = f[*o]; o++; }
	while (--s);
	QSORT(dim, c + 1, *c, LT);
}

size_t dslyce(dim id, dim m, const dim *l, vector<vector<var>> &vars, const dim *desc) {

	dim *r = (dim *)malloc(sizeof(dim) * (K + 1) * N);
	dim *f = (dim *)malloc(sizeof(dim) * (N + 1) * N);
	dim *ft = (dim *)malloc(sizeof(dim) * N);
	dim a[3] = {1, id, 0};
	size_t ret = coalition(a, vars, desc);

        for (dim i = 0; i < N; i++) {

                a[1] = i;
                nbar(a + 1, 1, a + 2, a, l, f);

                for (dim k = 1; k <= MIN(*f, m - 1); k++) {
			size_t bc = B(*f, k);
			dim j = bc * id / N;
                        while (j < bc * (id + 1) / N) {
				li(f, j + 1, k, ft);
				j++;
				unionsorted(a + 1, (dim)1, ft + 1, *ft, r + 1, r);
				nbar(ft + 1, k, a, r, l, f + N + 1);
				ret += slyce(r, f + N + 1, m - (k + 1), l, vars, desc);
                        }

                        id = (id + 1) % N;
                }
        }

	free(ft);
	free(f);
	free(r);
        return ret;
}

size_t enumerate(const chunk *adj, vector<vector<var>> &vars, const dim *desc) {

	dim l[N * N], r[(K + 1) * N], f[(N + 1) * N];
	adjacencylist(adj, l);
	size_t ret = 0;

        for (dim i = 0; i < N; i++) {
                r[0] = 0; f[0] = 1; f[1] = i;
		ret += slyce(r, f, K, l, vars, desc);
        }

	return ret;
}

size_t enumeratedslyce(const chunk *adj, vector<vector<var>> &vars, const dim *desc) {

	filltables();
        dim l[N * N];
	adjacencylist(adj, l);
	printf("%u threads\n", T);
        size_t ret = 0, c[T];
	memset(c, 0, sizeof(size_t) * T);
	vector<vector<vector<var>>> ut(T, vector<vector<var>>(N, vector<var>()));

	#pragma omp parallel for schedule(dynamic)
	for (dim i = 0; i < N; i++)
		c[omp_get_thread_num()] += dslyce(i, K, l, ut[omp_get_thread_num()], desc);

	for (dim t = 0; t < T; t++) {
		ret += c[t];
		for (dim i = 0; i < N; i++)
			vars[i].insert(vars[i].end(), ut[t][i].begin(), ut[t][i].end());
	}

        return ret;
}
