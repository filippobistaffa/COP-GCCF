#ifndef DSLYCE_H_
#define DSLYCE_H_

void printc(const dim *c, value v);

size_t enumerate(const chunk *adj, vector<vector<var>> &vars, const dim *desc);

size_t enumeratedslyce(const chunk *adj, const vector<vector<var>> &vars, const dim *desc);

#endif /* DSLYCE_H_ */
