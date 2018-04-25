#include "copgccf.h"

void showgraph(const chunk *adj) {

	netstream::NetStreamSender *ns = new netstream::NetStreamSender(PRIMALPORT);
	ostringstream s1, s2, s3;
	chunk tmp[CN];

	struct timeval tp;
	gettimeofday(&tp, NULL);
	long time = tp.tv_sec * 1000 + tp.tv_usec / 1000;

	ns->graphClear("", time++);
	ns->addGraphAttribute("", time++, "ui.antialias", "");
	s1.str(STYLESHEET);
	ns->addGraphAttribute("", time++, "ui.stylesheet", s1.str());
	s1.str("1");
	ns->addGraphAttribute("", time++, "layout.stabilization-limit", s1.str());
	s1.str("4");
	ns->addGraphAttribute("", time++, "layout.quality", s1.str());

	for (dim i = 0; i < N; i++) {
		s1.str("");
		s1 << i;
		ns->addNode("", time++, s1.str());
		ns->addNodeAttribute("", time++, s1.str(), "ui.label", s1.str());
	}

	for (dim i = 0; i < N; i++) {
		memcpy(tmp, adj + i * CN, sizeof(chunk) * CN);
		const dim popc = MASKPOPCNT(tmp, CN);
		for (dim j = 0, k = MASKFFS(tmp, CN); j < popc; j++, k = MASKCLEARANDFFS(tmp, k, CN))
			if (i < k) {
				s1.str(""); s1 << i;
				s2.str(""); s2 << k;
				s3.str(""); s3 << i << "--" << k;
				ns->addEdge("", time++, s3.str(), s1.str(), s2.str(), 0);
			}
	}

	delete ns;
}

void showpseudotree(const dim *pt, dim root) {

	netstream::NetStreamSender *ns = new netstream::NetStreamSender(PSEUDOTREEPORT);
	ostringstream s1, s2, s3;

	struct timeval tp;
	gettimeofday(&tp, NULL);
	long time = tp.tv_sec * 1000 + tp.tv_usec / 1000;

	ns->graphClear("", time++);
	ns->addGraphAttribute("", time++, "ui.antialias", "");
	s1.str(STYLESHEET);
	ns->addGraphAttribute("", time++, "ui.stylesheet", s1.str());
	s1.str("1");
	ns->addGraphAttribute("", time++, "layout.stabilization-limit", s1.str());
	s1.str("4");
	ns->addGraphAttribute("", time++, "layout.quality", s1.str());

	for (dim i = 0; i < N; i++) {
		s1.str("");
		s1 << i;
		ns->addNode("", time++, s1.str());
		ns->addNodeAttribute("", time++, s1.str(), "ui.label", s1.str());
	}

	s1.str("");
	s1 << root;
	s2.str("root");
	ns->addNodeAttribute("", time++, s1.str(), "ui.class", s2.str());

	for (dim i = 0; i < N; i++)
		for (dim j = 0; j < pt[i * N]; j++) {
			s1.str(""); s1 << i;
			s2.str(""); s2 << pt[i * N + j + 1];
			s3.str(""); s3 << i << "--" << pt[i * N + j + 1];
			ns->addEdge("", time++, s3.str(), s1.str(), s2.str(), 1);
		}

	delete ns;
}
