#ifndef INDUCED_H_
#define INDUCED_H_

dim *greedyorder(chunk *adj);

dim *noorder();

dim inducedwidth(const chunk *adj, const dim *order);

#endif /* INDUCED_H_ */
