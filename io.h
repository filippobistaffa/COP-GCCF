#ifndef IO_H_
#define IO_H_

#include <iostream>
template <typename type>
__attribute__((always_inline)) inline
void printbuf(const type *buf, unsigned n, const char *name = NULL) {

	if (name) printf("%s = [ ", name);
	else printf("[ ");
	while (n--) std::cout << *(buf++) << " ";
	printf("]\n");
}

void printadj(const chunk *adj);

void printcost(const cost *c);

#ifdef SCALEFREE
void createscalefree(chunk *adj);
#endif

#ifdef COMPLETE
void createcomplete(chunk *adj);
#endif

void writewcspfile(const cost *ca, dim nv, dim seed, const char *filename);

void writeorderfile(const vector<vector<var>> &vars, const dim *pt, const char *filename);

#endif /* IO_H_ */
