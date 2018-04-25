#ifndef UI_H_
#define UI_H_

#include <time.h>
#include <sstream>
#include "ui/gs-netstream/c++/src/netstream-sender.h"

#define PRIMALPORT 2001
#define PSEUDOTREEPORT 2002
#define STYLESHEET "url('file:///home/filippo/phd/cop-gccf/ui/style.css')"

void showgraph(const chunk *adj);

void showpseudotree(const dim *pt, dim root);

#endif /* UI_H_ */
