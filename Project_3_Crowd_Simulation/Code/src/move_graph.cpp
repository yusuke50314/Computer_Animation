
# include "move_graph.h"

MoveGraph::MoveGraph ()
{
}

static KnMotion* load_motion ( KnSkeleton* sk, const char* filename )
{
	KnMotion* m = new KnMotion;
	if ( !m->load ( filename ) ) gsout.fatal("Error loading [%s]!",filename);
	m->connect ( sk );
	m->ref ();
	return m;
}

void MoveGraph::init ( KnSkeleton* sk )
{
	nodes.push()->motion = load_motion(sk, "../data/walking.bvh"); // 0

	Node* n;

	// motion node 0:
	n=nodes[0]; n->nextnode=0;
	n->anytimetransitions=true;

}
