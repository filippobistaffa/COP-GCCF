#ifndef CUCOP_H_
#define CUCOP_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>

#ifdef PARALLEL
#define COALITIONS enumeratedslyce
#else
#define COALITIONS enumerate
#endif

// https://troydhanson.github.io/uthash/

#include "uthash/src/uthash.h"

// For vectors
#include <vector>
#include <algorithm>
using namespace std;
#define ITOP(X) (&(*(X)))

// Parameters

#include "params.h"
#include "instance.h"
#include "types.h"

// Utility

#include "colours.h"
#include "macros.h"
#include "iqsort.h"
#include "sorted.h"

// Modules

#include "io.h"
#include "ui.h"
#include "random.h"
#include "induced.h"
#include "pseudotree.h"
#include "dslyce.h"
#include "req.h"

#endif /* CUCOP_H_ */
