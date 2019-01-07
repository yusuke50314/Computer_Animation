
/** Viewer Header**/
# include "my_viewer.h"

/** Particle System Header **/
# include "particle_system.h"

/** System-level Application Control Functions **/
# include <sigogl/ws_run.h>

int main ( int argc, char** argv )
{
	

	// Initialization
	ParticleSystem ps;
	MyViewer* v = new MyViewer(-1, -1, 800, 600, "EECS287 Final Project");
	//GsQuat q;
	//q.set(GsVec::i, float(-GS_PI/2));

	// Particle System
	ps.init ( "../src/config.txt" );

	// Viewer
	v->build_ui ();				// ui has to be built before scene
	v->build_scene ( ps, 12);	// 12 is the triangle resolution
	v->view_all ();
	v->show ();
	//v->rotate_camera(q);

	// System Process
	ws_run ();

	return 1;
}
