import java.io.IOException;
import java.net.UnknownHostException;

import org.graphstream.graph.Graph;
import org.graphstream.graph.implementations.MultiGraph;
import org.graphstream.stream.thread.ThreadProxyPipe;
import org.graphstream.stream.netstream.NetStreamReceiver;
import org.graphstream.ui.layout.HierarchicalLayout;
import org.graphstream.ui.view.Viewer;

/**
 *  A simple example of use of the NetStream receiver.
 */
public class Receiver {

	public static void main(String[] args) throws UnknownHostException, IOException, InterruptedException {
		System.setProperty("org.graphstream.ui.renderer", "org.graphstream.ui.j2dviewer.J2DGraphRenderer");
		// ----- On the receiver side -----
		//
		// - a graph that will display the received events
		Graph g = new MultiGraph("G");
		Viewer viewer = g.display();
		// - the receiver that waits for events
		NetStreamReceiver net = new NetStreamReceiver(Integer.parseInt(args[0]));
		// - received events end up in the "default" pipe
		ThreadProxyPipe pipe = net.getDefaultStream();
		// - plug the pipe to the sink of the graph
		pipe.addSink(g);
		// -The receiver pro-actively checks for events on the ThreadProxyPipe
		while (true) {
			pipe.pump();
			Thread.sleep(100);
		}
	}
}
