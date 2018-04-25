#ifndef INSTANCE_H_
#define INSTANCE_H_

#ifdef TWITTER
#include "graph.h"
#else
#define N 10 // number of agents
#endif

#ifdef SCALEFREE
#define CREATEGRAPH createscalefree
#define M 2 // parameter m in BA model
#define E (M * N - (M * (M + 1)) / 2)
#endif

#ifdef COMPLETE
#define CREATEGRAPH createcomplete
#define E ((N * (N - 1)) / 2)
#endif

#define K N

#define MAXV (10)
#define UNVALID (1E6)

#define CN CEILBPC(N)

#endif /* INSTANCE_H_ */

