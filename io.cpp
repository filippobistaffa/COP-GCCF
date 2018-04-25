#include "copgccf.h"

#define WIDTH "3"
#define FORMAT "%" WIDTH "u"
#include <iostream>

void printadj(const chunk *adj) {

	printf("%" WIDTH "s", " ");

	for (dim i = 0; i < N; i++)
		printf(CYAN(FORMAT), i);

	printf("\n");

	for (dim i = 0; i < N; i++) {
		printf(CYAN(FORMAT), i);
		for (dim j = 0; j < N; j++)
			printf(BITFORMAT, GET(adj + i * CN, j));
		printf("\n");
	}
}

void printcost(const cost *c) {

	for (dim i = 0; i < c->a; i++)
		printf(YELLOW(FORMAT), i);

	printf("\n");

	for (dim i = 0; i < c->a; i++)
		printf(CYAN(FORMAT), c->vars[i]);

	std::cout << " (default = " << c->d << ")" << std::endl;

	for (dim i = 0; i < c->n; i++) {
		for (dim j = 0; j < c->a; j++)
			printf(FORMAT, c->r[i].x[j]);
		std::cout << " = " << c->r[i].v << std::endl;
	}
}

#ifdef SCALEFREE
void createscalefree(chunk *adj) {

	dim deg[N] = {0};
	chunk t = 0;

	for (dim i = 1; i <= M; i++) {
		for (dim j = 0; j < i; j++) {
			EDGE(adj, CN, i, j);
			deg[i]++;
			deg[j]++;
		}
	}

	for (dim i = M + 1; i < N; i++) {
		t &= ~((ONE << i) - 1);
		for (dim j = 0; j < M; j++) {
			dim d = 0;
			for (dim h = 0; h < i; h++)
				if (!((t >> h) & 1)) d += deg[h];
			if (d > 0) {
				int p = nextInt(d);
				dim q = 0;
				while (p >= 0) {
					if (!((t >> q) & 1)) p = p - deg[q];
					q++;
				}
				q--;
				t |= ONE << q;
				EDGE(adj, CN, i, q);
				deg[i]++;
				deg[q]++;
			}
		}
	}
}
#endif

#ifdef COMPLETE
void createcomplete(chunk *adj) {

	for (dim i = 0; i < N; i++)
		for (dim j = i + 1; j < N; j++)
			EDGE(adj, CN, i, j);
}
#endif

#include <fstream>
using namespace std;

void writewcspfile(const cost *ca, dim nv, dim seed, const char *filename) {

	ofstream file;
	file.open(filename);

	// First wcsp row
	file << N << "-" << seed << " " << nv << " " << 2 << " " << N << " " << UNVALID - 1 << endl;

	// Second wcsp row
	file << 2;
	for (dim i = 1; i < nv; i++)
		file << " " << 2;
	file << endl;

	// Cost functions
	for (dim i = 0; i < N; i++) {

		file << ca[i].a;
		for (dim j = 0; j < ca[i].a; j++)
			file << " " << ca[i].vars[j];
		file << " " << ca[i].d << " " << ca[i].n << endl;

		for (dim j = 0; j < ca[i].n; j++) {
			for (dim k = 0; k < ca[i].a; k++)
				file << ca[i].r[j].x[k] << " ";
			file << ca[i].r[j].v << endl;
		}
	}

	file.close();
}

void writeorderfile(const vector<vector<var>> &vars, const dim *pt, const char *filename) {

	ofstream file;
	file.open(filename);

	for (dim i = 0; i < N; i++)
		for (vector<var>::const_iterator it = vars[i].begin(); it != vars[i].end(); ++it)
			file << ITOP(it)->i << endl;

	file.close();
}
