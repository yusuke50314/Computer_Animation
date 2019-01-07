
# include "move_graph.h"

# include <sig/gs_event.h>


MoveGraph::MoveGraph ()
{
}

static KnMotion* load_motion ( KnSkeleton* sk, const char* filename )
{
	KnMotion* m = new KnMotion;
	if ( !m->load ( filename ) ) gsout.fatal("Error loading [%s]!",filename);
	m->connect ( sk );																		// Connect with Skeleton Model
	m->ref ();
	return m;
}

void MoveGraph::init ( KnSkeleton* sk )
{
	int speed = 0;
	// 0: Normal Speed Rotation, 1: Higher Speed Rotation, 2: Lower Speed Rotation

	nodes.push()->motion = load_motion(sk, "../data/motion/Idle.bvh"); // 0

	if (speed == 0) {
		nodes.push()->motion = load_motion(sk, "../data/motion/LeftTurn180.bvh"); // 1
	}
	else if(speed==1){
		nodes.push()->motion = load_motion(sk, "../data/motion/LeftTurn180Double.bvh"); // 1
	}
	else if (speed == 2) {
		nodes.push()->motion = load_motion(sk, "../data/motion/LeftTurn180Half.bvh"); // 1
	}

	if (speed == 0) {
		nodes.push()->motion = load_motion(sk, "../data/motion/RightTurn180.bvh"); // 2
	}
	else if (speed == 1) {
		nodes.push()->motion = load_motion(sk, "../data/motion/RightTurn180Double.bvh"); // 2
	}
	else if (speed == 2) {
		nodes.push()->motion = load_motion(sk, "../data/motion/RightTurn180Half.bvh"); // 2
	}

	nodes.push()->motion = load_motion ( sk, "../data/motion/StandardWalk.bvh" ); // 3
	nodes.push()->motion = load_motion ( sk, "../data/motion/StandardRun.bvh" ); // 4
	nodes.push()->motion = load_motion ( sk, "../data/motion/SneakingIdle.bvh" ); // 5
	nodes.push()->motion = load_motion(sk, "../data/motion/SneakingWalkSlow.bvh"); // 6
	nodes.push()->motion = load_motion(sk, "../data/motion/LeftTurnSneaking.bvh"); // 7
	nodes.push()->motion = load_motion(sk, "../data/motion/RightTurnSneaking.bvh"); // 8
	nodes.push()->motion = load_motion(sk, "../data/motion/StandardWalk.bvh"); // 9
	nodes.push()->motion = load_motion(sk, "../data/motion/StandardWalk.bvh"); // 10
	nodes.push()->motion = load_motion(sk, "../data/motion/StandardRun.bvh"); // 11
	nodes.push()->motion = load_motion(sk, "../data/motion/StandardRun.bvh"); // 12
	nodes.push()->motion = load_motion(sk, "../data/motion/Jumping.bvh"); // 13
	nodes.push()->motion = load_motion(sk, "../data/motion/RunToJump.bvh"); // 14
	nodes.push()->motion = load_motion(sk, "../data/motion/WalkToRun.bvh"); // 15
	nodes.push()->motion = load_motion(sk, "../data/motion/SneakingWalkSlow.bvh"); // 16
	nodes.push()->motion = load_motion(sk, "../data/motion/SneakingWalkSlow.bvh"); // 17

	nodes.push()->motion = load_motion(sk, "../data/motion/Idle.bvh"); // 18
	nodes.push()->motion = load_motion(sk, "../data/motion/Idle.bvh"); // 19

	Node* n;
	Transition* t;
	int motion_select = 1;

	if (motion_select == 0) {

		// motion node 0:
		n = nodes[0]; n->nextnode = 0;
		n->anytimetransitions = true;			// true: now, false: reserve
		t = &n->trans.push(); t->key = GsEvent::KeyUp; t->nextnode = 1;

		// motion node 1:
		n = nodes[1]; n->nextnode = 2;
		n->anytimetransitions = false;

		// motion node 2:
		n = nodes[2]; n->nextnode = 3;
		n->anytimetransitions = false;
		t = &n->trans.push(); t->key = GsEvent::KeyDown; t->nextnode = 5;

		// motion node 3:
		n = nodes[3]; n->nextnode = 2;
		n->anytimetransitions = false;
		t = &n->trans.push(); t->key = GsEvent::KeyLeft; t->nextnode = 4;

		// motion node 4:
		n = nodes[4]; n->nextnode = 0;
		n->anytimetransitions = false;

		// motion node 5:
		n = nodes[5]; n->nextnode = 3;
		n->anytimetransitions = false;

	} else if (motion_select == 1) {

		// motion node 0:
		n = nodes[0]; n->nextnode = 0;
		n->anytimetransitions = true;			// true: now, false: reserve
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = GsEvent::KeyUp; t->nextnode = 3;
		t = &n->trans.push(); t->key = GsEvent::KeyDown; t->nextnode = 5;
		t = &n->trans.push(); t->key = GsEvent::KeyLeft; t->nextnode = 1;
		t = &n->trans.push(); t->key = GsEvent::KeyRight; t->nextnode = 2;
		t = &n->trans.push(); t->key = GsEvent::KeySpace; t->nextnode = 13;

		// motion node 1:
		n = nodes[1]; n->nextnode = 1;
		n->anytimetransitions = true;
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = 1; t->nextnode = 0;

		// motion node 2:
		n = nodes[2]; n->nextnode = 2;
		n->anytimetransitions = true;
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = 1; t->nextnode = 0;

		// motion node 3:
		n = nodes[3]; n->nextnode = 3;
		n->anytimetransitions = true;
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = 'z'; t->nextnode = 15;
		t = &n->trans.push(); t->key = 1; t->nextnode = 0;
		t = &n->trans.push(); t->key = GsEvent::KeyLeft; t->nextnode = 9;
		t = &n->trans.push(); t->key = GsEvent::KeyRight; t->nextnode = 10;

		// motion node 15:
		n = nodes[15]; n->nextnode = 4;
		n->anytimetransitions = true;
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = 1; t->nextnode = 3;

		// motion node 4:
		n = nodes[4]; n->nextnode = 4;
		n->anytimetransitions = true;
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = 1; t->nextnode = 3;
		t = &n->trans.push(); t->key = GsEvent::KeyLeft; t->nextnode = 11;
		t = &n->trans.push(); t->key = GsEvent::KeyRight; t->nextnode = 12;
		t = &n->trans.push(); t->key = GsEvent::KeySpace; t->nextnode = 14;

		// motion node 5:
		n = nodes[5]; n->nextnode = 5;
		n->anytimetransitions = true;
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = GsEvent::KeyUp; t->nextnode = 6;
		t = &n->trans.push(); t->key = GsEvent::KeyDown; t->nextnode = 0;
		t = &n->trans.push(); t->key = GsEvent::KeyLeft; t->nextnode = 7;
		t = &n->trans.push(); t->key = GsEvent::KeyRight; t->nextnode = 8;

		// motion node 6:
		n = nodes[6]; n->nextnode = 6;
		n->anytimetransitions = true;
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = 1; t->nextnode = 5;
		t = &n->trans.push(); t->key = GsEvent::KeyLeft; t->nextnode = 16;
		t = &n->trans.push(); t->key = GsEvent::KeyRight; t->nextnode = 17;

		// motion node 7:
		n = nodes[7]; n->nextnode = 7;
		n->anytimetransitions = true;
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = 1; t->nextnode = 5;

		// motion node 8:
		n = nodes[8]; n->nextnode = 8;
		n->anytimetransitions = true;
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = 1; t->nextnode = 5;

		// motion node 9:
		n = nodes[9]; n->nextnode = 9;
		n->anytimetransitions = true;
		n->rotationchecker = 1;
		t = &n->trans.push(); t->key = 1; t->nextnode = 3;

		// motion node 10:
		n = nodes[10]; n->nextnode = 10;
		n->anytimetransitions = true;
		n->rotationchecker = 2;
		t = &n->trans.push(); t->key = 1; t->nextnode = 3;

		// motion node 11:
		n = nodes[11]; n->nextnode = 11;
		n->anytimetransitions = true;
		n->rotationchecker = 1;
		t = &n->trans.push(); t->key = 1; t->nextnode = 4;

		// motion node 12:
		n = nodes[12]; n->nextnode = 12;
		n->anytimetransitions = true;
		n->rotationchecker = 2;
		t = &n->trans.push(); t->key = 1; t->nextnode = 4;

		// motion node 13:
		n = nodes[13]; n->nextnode = 13;
		n->anytimetransitions = false;
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = 1; t->nextnode = 0;

		// motion node 14:
		n = nodes[14]; n->nextnode = 14;
		n->anytimetransitions = false;
		n->rotationchecker = 0;
		t = &n->trans.push(); t->key = 1; t->nextnode = 4;

		// motion node 16:
		n = nodes[16]; n->nextnode = 16;
		n->anytimetransitions = false;
		n->rotationchecker = 1;
		t = &n->trans.push(); t->key = 1; t->nextnode = 6;

		// motion node 17:
		n = nodes[17]; n->nextnode = 17;
		n->anytimetransitions = false;
		n->rotationchecker = 2;
		t = &n->trans.push(); t->key = 1; t->nextnode = 6;

	}
	else if (motion_select == 2) {

	// motion node 0:
	n = nodes[0]; n->nextnode = 0;
	n->anytimetransitions = true;			// true: now, false: reserve
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = GsEvent::KeyUp; t->nextnode = 3;
	t = &n->trans.push(); t->key = GsEvent::KeyDown; t->nextnode = 5;
	t = &n->trans.push(); t->key = GsEvent::KeyLeft; t->nextnode = 18;
	t = &n->trans.push(); t->key = GsEvent::KeyRight; t->nextnode = 19;
	t = &n->trans.push(); t->key = GsEvent::KeySpace; t->nextnode = 13;

	// motion node 1:
	n = nodes[1]; n->nextnode = 1;
	n->anytimetransitions = true;
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = 1; t->nextnode = 0;

	// motion node 2:
	n = nodes[2]; n->nextnode = 2;
	n->anytimetransitions = true;
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = 1; t->nextnode = 0;

	// motion node 3:
	n = nodes[3]; n->nextnode = 3;
	n->anytimetransitions = true;
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = 'z'; t->nextnode = 15;
	t = &n->trans.push(); t->key = 1; t->nextnode = 0;
	t = &n->trans.push(); t->key = GsEvent::KeyLeft; t->nextnode = 9;
	t = &n->trans.push(); t->key = GsEvent::KeyRight; t->nextnode = 10;

	// motion node 15:
	n = nodes[15]; n->nextnode = 4;
	n->anytimetransitions = true;
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = 1; t->nextnode = 3;

	// motion node 4:
	n = nodes[4]; n->nextnode = 4;
	n->anytimetransitions = true;
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = 1; t->nextnode = 3;
	t = &n->trans.push(); t->key = GsEvent::KeyLeft; t->nextnode = 11;
	t = &n->trans.push(); t->key = GsEvent::KeyRight; t->nextnode = 12;
	t = &n->trans.push(); t->key = GsEvent::KeySpace; t->nextnode = 14;

	// motion node 5:
	n = nodes[5]; n->nextnode = 5;
	n->anytimetransitions = true;
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = GsEvent::KeyUp; t->nextnode = 6;
	t = &n->trans.push(); t->key = GsEvent::KeyDown; t->nextnode = 0;
	t = &n->trans.push(); t->key = GsEvent::KeyLeft; t->nextnode = 7;
	t = &n->trans.push(); t->key = GsEvent::KeyRight; t->nextnode = 8;

	// motion node 6:
	n = nodes[6]; n->nextnode = 6;
	n->anytimetransitions = true;
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = 1; t->nextnode = 5;
	t = &n->trans.push(); t->key = GsEvent::KeyLeft; t->nextnode = 16;
	t = &n->trans.push(); t->key = GsEvent::KeyRight; t->nextnode = 17;

	// motion node 7:
	n = nodes[7]; n->nextnode = 7;
	n->anytimetransitions = true;
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = 1; t->nextnode = 5;

	// motion node 8:
	n = nodes[8]; n->nextnode = 8;
	n->anytimetransitions = true;
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = 1; t->nextnode = 5;

	// motion node 9:
	n = nodes[9]; n->nextnode = 9;
	n->anytimetransitions = true;
	n->rotationchecker = 1;
	t = &n->trans.push(); t->key = 1; t->nextnode = 3;

	// motion node 10:
	n = nodes[10]; n->nextnode = 10;
	n->anytimetransitions = true;
	n->rotationchecker = 2;
	t = &n->trans.push(); t->key = 1; t->nextnode = 3;

	// motion node 11:
	n = nodes[11]; n->nextnode = 11;
	n->anytimetransitions = true;
	n->rotationchecker = 1;
	t = &n->trans.push(); t->key = 1; t->nextnode = 4;

	// motion node 12:
	n = nodes[12]; n->nextnode = 12;
	n->anytimetransitions = true;
	n->rotationchecker = 2;
	t = &n->trans.push(); t->key = 1; t->nextnode = 4;

	// motion node 13:
	n = nodes[13]; n->nextnode = 13;
	n->anytimetransitions = false;
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = 1; t->nextnode = 0;

	// motion node 14:
	n = nodes[14]; n->nextnode = 14;
	n->anytimetransitions = false;
	n->rotationchecker = 0;
	t = &n->trans.push(); t->key = 1; t->nextnode = 4;

	// motion node 16:
	n = nodes[16]; n->nextnode = 16;
	n->anytimetransitions = false;
	n->rotationchecker = 1;
	t = &n->trans.push(); t->key = 1; t->nextnode = 6;

	// motion node 17:
	n = nodes[17]; n->nextnode = 17;
	n->anytimetransitions = false;
	n->rotationchecker = 2;
	t = &n->trans.push(); t->key = 1; t->nextnode = 6;

	// motion node 18:
	n = nodes[18]; n->nextnode = 18;
	n->anytimetransitions = true;
	n->rotationchecker = 3;
	t = &n->trans.push(); t->key = 1; t->nextnode = 0;

	// motion node 19:
	n = nodes[19]; n->nextnode = 19;
	n->anytimetransitions = true;
	n->rotationchecker = 4;
	t = &n->trans.push(); t->key = 1; t->nextnode = 0;

	}
}
