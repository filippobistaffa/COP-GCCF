#ifndef PSEUDOTREE_H_
#define PSEUDOTREE_H_

void adjacencylist(const chunk *adj, dim *l);

dim pseudotree(const chunk *adj, const dim *order, dim *pt);

void descendants(dim i, dim *pt, dim *desc);

#endif /* PSEUDOTREE_H_ */
