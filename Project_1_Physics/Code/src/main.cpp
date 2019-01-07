
# include "my_viewer.h"
# include "particle_system.h"

# include <sigogl/ws_run.h>

int main ( int argc, char** argv )
{
	// 1. Initialize particle system:
	ParticleSystem ps;
	ps.init ( "../src/config.txt" ); //Loading file

	// 2. Create viewer and build scene:
	MyViewer* v = new MyViewer ( -1, -1, 800, 600, "EECS287 Support Code - Example Particle System" );
	v->build_ui ();				// ui has to be built before scene
	v->build_scene ( ps, 12 );	// 12 is the triangle resolution //���Ɏg����O�p�`
	v->view_all ();
	v->show ();

	// 3. Enter main loop:
	ws_run ();

	return 1;
}
