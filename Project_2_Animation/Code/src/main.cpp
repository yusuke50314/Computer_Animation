# include "my_viewer.h"
# include "move_graph.h"

# include <sig/gs_euler.h>
# include <sig/gs.h>
# include <sigogl/ws_run.h>
# include <sig/gs_geo2.h>
# include <sigogl/ui_dialogs.h>

#include <Windows.h>
// Windows Keboard Input Function

# include <sigkin/kn_fbx_importer.h>

int main ( int argc, char** argv )
{
	
	// 1. Load skeleton definition:
	KnSkeleton* sk = new KnSkeleton();		// Skeleton Structure
	KnFbxImporter fbximp;					// Skeleton + Texture Structure

	gsout << "Before: " << sk->root() << gsnl;
	
	//if (!sk->load("../data/hdman.s")) {				// Loading File
	//	gsout.fatal("Error loading skeleton!");		// Error Message
	//}	
	
	if (!fbximp.load("../data/FBX 2012/remy.fbx")){
		gsout.fatal("Error loading skeleton!");
	}else {
		fbximp.get_skeleton(sk);
	}
	gsout << "After: " << sk->root() << gsnl;
	
	KnJoint* rootj = sk->root();								// Root of Skeleton
	if ( !rootj ) gsout.fatal("Could not find root joint!");	// Error Message

	//gsout << rootj->pos()->value().y << gsnl;
	float root_y = rootj->pos()->value().y;
	//213.254

	// 2. Create move graph and load all needed motions:
	MoveGraph* mg = new MoveGraph;								// Motion Graph
	mg->init ( sk );											// Load motions (compatible with skeleton sk)

	// 3. Create viewer and build scene:
	MyViewer* v = new MyViewer (sk, -1, -1, 800, 600, "Example Move Graph App" );		// Viewer Setting
	//MySkelViewer* viewer = new MySkelViewer(sk, -1, -1, 800, 600, "SIG Skelviewer");	// FBX Viewer

	bool addfloor = true;					// Floor Activation Bool
	v->build_scene ( sk, addfloor );		// Building the Scene

	v->view_all ();		// Viewing Composition
	v->show ();			// Show the Window

	
	// 4. Enter main loop
	// The integer below triggers examples of how to do different things, study one at a time:
	// 1:just display character, 2:play one motion, 3:test move graph, 4:move graph with blending
	int mainloop=5;

	//=================================================================
	if ( mainloop==1 ) // just display the character:
	{	ws_run ();
	}
	
	//=================================================================
	if ( mainloop==2 ) // just play one motion continously:
	{	KnMotion* curm = mg->nodes[5]->motion;		// Motion 1

		float mduration = curm->duration();			// Duration Motion 1
		gsout << "motion duration:" << mduration << gsnl;					// Debugging

		double st = gs_time();						// Start Time for 1st motion
		//gsout<<st<<gsnl;							// Debugging

		while ( true )
		{	// Compute local time:
			float localt = float(gs_time()-st);		// Local Time

			// When local time goes beyond the mduration we start again:
			if ( localt > mduration )				
			{	st = st + mduration;				// Update Start Time
				localt = localt - mduration;		// Local Time to zero again
				// gsout << st << gsnl;
				// gsout << "local time:" << localt << gsnl;
			}

			// Now apply motion at the current local time, update scene, and draw:
			curm->apply ( localt );					// Compute with Skeleton
			v->sk_scene()->update();				// Update Scene
			v->message().setf ( "loct=%4.2f  dur=%4.2f", localt, curm->duration() );
			v->render();
			ws_check(); // this updates window and interface (otherwise application will be unresponsive)
		}
	}

	//=================================================================
	if ( mainloop==3 ) // Concatenate motions
	{

		GsVec pg, pl, ppre, ppost;			// global and local root joint position
		GsQuat qg, ql, qpre, qpost, qr;			// global and local root joint orientation

		double st = gs_time();	// Start Time
		int state = 14;			// Current Node Number in the Move Graph

		MoveGraph::Node* curn = mg->nodes[state];	// Current Node in the Move Graph
		KnMotion* curm = curn->motion;				// Current Motion
		float curmdur = curm->duration();			// Current Motion's Duration

		int futurestate = -1; // new state to switch to after current one ends
		int newstatenow = -1; // new state to switch to right now

		while ( true )
		{	float localt = float(gs_time()-st);		// Local Time

			// if past end of current motion, go to next motion:
			if ( localt>curmdur )					// When local time goes beyond the current duration
			{	st = st + curmdur;					// Update Start Time
				localt = localt - curmdur;			// Local Time to zero again
				if (futurestate < 0) {
					futurestate = mg->nodes[state]->nextnode;	// Get Next Node
				}
				newstatenow = futurestate;						// Save Next Node
			}

			// check if there was a keyboard request to change state:
			if (v->lastkey() )
			{	for ( int i=0; i<curn->trans.size(); i++ )
				{	if ( v->lastkey()==curn->trans[i].key )
					{	futurestate = curn->trans[i].nextnode;		// Get Next Node from this key
						v->invalidate_lastkey();					// Last Key Remove
						if (curn->anytimetransitions) {				// Change Now
							newstatenow = futurestate;				// Save Next Node, without this it will go back
							st += localt;				// Update Start Time
							localt = 0;					// Local Time to zero again
						} 
					}
				}
			}

			if ( newstatenow>=0 ) // Update to Next Node
			{
				gsout << "UPDATE" << gsnl;
				state = newstatenow;				// Update Node Number 			
				curn = mg->nodes[state];			// Update Node 
				curm = curn->motion;				// Update Motion
				curmdur = curm->duration();			// Update Motion's Duration
				newstatenow = futurestate = -1;		// Initialize Condition Factor


				ppre = rootj->pos()->value();			// Root Global Position 
				qpre = rootj->rot()->value();
				curm->apply(0);
				ppost = rootj->pos()->value();			// Root Global Position
				qpost = rootj->rot()->value();

				pg = ppre-ppost;					// Root Global Position 
				pg.y=0;								// do not accumulate vertical offset

				float rx, ry, rz;
				gs_angles(gsYXZ, qpre, rx, ry, rz);		// Exploit Quotanion for just Y, Root Global Direction
				float rxI, ryI, rzI;
				gs_angles(gsYXZ, qpost, rxI, ryI, rzI);		// Exploit Quotanion for just Y, Root Global Direction
				qg.set(GsVec::j, ry - ryI);									// Direction
				
			}

			gsout << "Pre DIRECTION: ";
			gsout << rootj->rot()->value() << gsnl;
			// Now apply motion data, update to global, and draw:
			curm->apply ( localt );						// Compute with Skeleton
			gsout << "Post DIRECTION: ";
			gsout << rootj->rot()->value() << gsnl;

			pl = rootj->pos()->value ();				// Root Local Position 
			ql = rootj->rot()->value ();				// Root Local Direction

			if (curn->rotationchecker) {
				qr.set(GsVec::j, gspi / 24);									// Direction
			}
			else {
				qr.set(GsVec::j, 0);
			}

			gsout << "qr: " << qr << gsnl;

			gsout << "DIRECTION: ";
			gsout << rootj->rot()->value() << gsnl;
			gsout << "ql: " << ql << gsnl;
			gsout << "qg: " << qg << gsnl;

			rootj->pos()->value ( pg + qg.apply(pl) );	// Root Global Position + Rotated Root Local Position 
			rootj->rot()->value ( ql * qg * qr );			// Root Global Direction * Root Local Direction

			gsout << "DIRECTION: ";
			gsout << rootj->rot()->value() << gsnl;

			v->sk_scene()->update();					// Update Scene 
			v->message().setf ( "Keys: w,s,t  state=%d  loct=%4.2f  dur=%4.2f", state, localt, curm->duration() );
			v->render();

			ws_check(); // this updates window and interface (otherwise application will be unresponsive)
		}
	}

	//=================================================================
	if ( mainloop==4 ) // Concatenate motions with a simplistic blending
	{
		GsVec pg, pl;								// global and local root joint position
		GsQuat qg, ql;								// global and local root joint orientation

		double st = gs_time();						// Start Time
		int state = 0;								// Current Node Number in the Move Graph
		MoveGraph::Node* curn = mg->nodes[state];	// Current Node in the Move Graph
		KnMotion* curm = curn->motion;				// Current Motion
		float curmdur = curm->duration();			// Current Motion's Duration

		int futurestate = -1;						// new state to switch to after current one ends
		int newstatenow = -1;						// new state to switch to right now

		// Blending Setting
		KnPosture post1(sk), post2(sk), post3(sk);	// Postures to be used for Interpolation Blending


		post1.make_quat_channels();					// let's do all interpolations with quaternions
		post2.make_quat_channels();
		post3.make_quat_channels();

		

		float blwin = 0.2f; // blending window

		while ( true )
		{	float localt = float(gs_time()-st);		// Local Time

			// if past end of current motion, go to next motion:
			if (localt > curmdur)					// When local time goes beyond the current duration
			{
				st = st + curmdur;					// Update Start Time
				localt = localt - curmdur;			// Local Time to zero again
				if (futurestate < 0) {
					futurestate = mg->nodes[state]->nextnode;	// Get Next Node
				}
				newstatenow = futurestate;						// Save Next Node
			}


			// check if there was a keyboard request to change state:
			//if (GetKeyState(VK_SHIFT) && v->lastkey() ){
			if (v->lastkey()) {
				for ( int i=0; i<curn->trans.size(); i++ )
				{	if ( v->lastkey()==curn->trans[i].key )
					{	futurestate = curn->trans[i].nextnode;
						v->invalidate_lastkey();
						if (curn->anytimetransitions) {
							newstatenow = futurestate;
							st += localt;
							localt = 0;
						}
					}
				}
			}
			
			// Check if there is a new state requested:
			if ( newstatenow>=0 ){
				state = newstatenow;				// Update Node Number 			
				curn = mg->nodes[state];			// Update Node 
				curm = curn->motion;				// Update Motion
				curmdur = curm->duration();			// Update Motion's Duration
				newstatenow = futurestate = -1;		// Initialize Condition Factor
				
				post1.get(); // last posture of current motion will be used for concatenation blending

				gsout << "post1: " << gsnl;
				gsout << post1.values << gsnl;

				// update global pos:
				pg = rootj->pos()->value();			// Root Global Position 
				pg.y = 0;							// do not accumulate vertical offset

				float rx, ry, rz;
				gs_angles(gsYXZ, rootj->rot()->value(), rx, ry, rz);		// Exploit Quotanion for just Y, Root Global Direction
				//gs_angles_yxz(rootj->lmat(), rx, ry, rz, 'L');
				qg.set(GsVec::j, ry);									// Direction
			}

			// Now apply motion data, update to global, and draw:
			curm->apply(localt);						// Compute with Skeleton

			pl = rootj->pos()->value();				// Root Local Position 
			ql = rootj->rot()->value();				// Root Local Direction

			rootj->pos()->value(pg + qg.apply(pl));	// Root Global Position + Rotated Root Local Position 
			rootj->rot()->value(ql * qg);			// Root Global Direction * Root Local Direction

			// Do a very simple blending out from post 1 to improve concatenation:
			//if ( v->blend_but() && localt<blwin )
			if (localt < blwin)
			{	post2.get();							// 

				gsout << "post2: " << gsnl;
				gsout << post2.values << gsnl;

				float t = 1.0f-((blwin-localt)/blwin);	// t here varies from 0 to 1 in blending window
				interp ( post1, post2, t, post3 );		// 

				gsout << "post3: " << gsnl;
				gsout << post3.values << gsnl;

				post3.apply ();							// 
			}

			// Set camera:
			if ( v->follow_but() )
			{	v->camera().center = rootj->pos()->value();
				v->camera().eye = v->camera().center + (GsVec(0,0,150));
				v->camera().center.y = 50.0f;
				v->camera().eye.y = 80.0f;
			}

			v->sk_scene()->update();
			v->message().setf ( "Keys: w,s,t  state=%d  loct=%4.2f  dur=%4.2f", state, localt, curm->duration() );
			v->render();

			ws_check(); // this updates window and interface (otherwise application will be unresponsive)
		}
	}
	
	//=================================================================
	if (mainloop == 5) // Personal concatenate motions with a simplistic blending
	{
		GsVec plot_coord;
		GsVec pre_coord;


		plot_coord = rootj->pos()->value();
		plot_coord.y = 1;
		v->_object_plot->push(plot_coord);
		pre_coord = plot_coord;

		GsVec pg, pl, ppre, ppost;			// global and local root joint position
		GsQuat qg, ql, qpre, qpost, qr;			// global and local root joint orientation
		GsQuat dir_y;
		dir_y.set(GsVec::j, 0);

		double st = gs_time();						// Start Time
		double starting = gs_time();						// Start Time

		int state = 0;								// Current Node Number in the Move Graph
		MoveGraph::Node* curn = mg->nodes[state];	// Current Node in the Move Graph
		KnMotion* curm = curn->motion;				// Current Motion
		float curmdur = curm->duration();			// Current Motion's Duration

		int futurestate = -1;						// new state to switch to after current one ends
		int newstatenow = -1;						// new state to switch to right now

		// Blending Setting
		KnPosture post1(sk), post2(sk), post3(sk);	// Postures to be used for Interpolation Blending


		post1.make_quat_channels();					// let's do all interpolations with quaternions
		post2.make_quat_channels();
		post3.make_quat_channels();



		float blwin = 0.2f; // blending window
		float localt = 0;

		double point;
		GsVec2 _toward_goal;
		GsVec _stposition;
		int id = 0;

		_stposition.set(plot_coord.x, 0, plot_coord.z);
		point = sqrt(gs_point_segment_dist2(plot_coord.x, plot_coord.z, _stposition.x, _stposition.z, v->_flag_point->P[id].x, v->_flag_point->P[id].z));
		_toward_goal = GsVec2(plot_coord.x, plot_coord.z) - GsVec2(v->_flag_point->P[id].x, v->_flag_point->P[id].z);
		
		double sum_point = 0;
		double number_point = 0;
		double average_dist;

		GsVec _pre_pos = rootj->pos()->value();

		while (true)
		{
			//gsout << float(gs_time() - st)-localt << gsnl;

			localt = float(gs_time() - st);		// Local Time

		// if past end of current motion, go to next motion:
			if (localt > curmdur)					// When local time goes beyond the current duration
			{
				st = st + curmdur;					// Update Start Time
				localt = localt - curmdur;			// Local Time to zero again
				if (futurestate < 0) {
					futurestate = mg->nodes[state]->nextnode;	// Get Next Node
				}
				newstatenow = futurestate;						// Save Next Node
			}


			// check if there was a keyboard request to change state:
			//if (GetKeyState(VK_SHIFT) && v->lastkey() ){

			if (v->lastkey()) {
				for (int i = 0; i < curn->trans.size(); i++)
				{
					if (v->lastkey() == curn->trans[i].key)
					{
						futurestate = curn->trans[i].nextnode;
						v->invalidate_lastkey();
						if (curn->anytimetransitions) {
							newstatenow = futurestate;
							st += localt;
							localt = 0;
						}
					}
				}
			}

			// Check if there is a new state requested:
			if (newstatenow >= 0) {
				state = newstatenow;				// Update Node Number 			
				curn = mg->nodes[state];			// Update Node 
				curm = curn->motion;				// Update Motion
				curmdur = curm->duration();			// Update Motion's Duration
				newstatenow = futurestate = -1;		// Initialize Condition Factor

				post1.get(); // last posture of current motion will be used for concatenation blending

				//gsout << "post1: " << gsnl;
				//gsout << post1.values << gsnl;

				ppre = rootj->pos()->value();			// Root Global Position 
				qpre = rootj->rot()->value();
				curm->apply(0);
				ppost = rootj->pos()->value();			// Root Global Position
				qpost = rootj->rot()->value();

				pg = ppre - ppost;					// Root Global Position 
				pg.y = 0;								// do not accumulate vertical offset

				float rx, ry, rz;
				gs_angles(gsYXZ, qpre, rx, ry, rz);		// Exploit Quotanion for just Y, Root Global Direction
				float rxI, ryI, rzI;
				gs_angles(gsYXZ, qpost, rxI, ryI, rzI);		// Exploit Quotanion for just Y, Root Global Direction
				qg.set(GsVec::j, ry - ryI);									// Direction
				if (curn->rotationchecker == 3) {
					//qg.set(GsVec::j, ry);
				}
			}

			// Now apply motion data, update to global, and draw:
			curm->apply(localt);						// Compute with Skeleton

			pl = rootj->pos()->value();				// Root Local Position 
			float rx, ry, rz;
			gs_angles_yxz(rootj->lmat(), rx, ry, rz, 'L');
			
			ql.set(GsVec::j, ry);
			//ql = rootj->rot()->value();				// Root Local Direction

			if (curn->rotationchecker==1) {
				qr.set(GsVec::j, gspi / 9);									// Direction
			}
			else if (curn->rotationchecker == 2) {
				qr.set(GsVec::j, -gspi / 9);
			}
			else if (curn->rotationchecker == 3) {
				qr.set(GsVec::j, gspi / 6);
			}
			else if (curn->rotationchecker == 4) {
				qr.set(GsVec::j, -gspi / 6);
			}
			else {
				qr.set(GsVec::j, 0);
			}

			
			if (curn->rotationchecker == 3 || curn->rotationchecker == 4) {
				rootj->pos()->value(_pre_pos);
			}
			else {
				rootj->pos()->value(pg + qg.apply(pl));	// Root Global Position + Rotated Root Local Position 
			}
			
			rootj->rot()->value(ql * qg * qr);			// Root Global Direction * Root Local Direction
			_pre_pos = rootj->pos()->value();

			

			// Do a very simple blending out from post 1 to improve concatenation:
			//if ( v->blend_but() && localt<blwin )
			if (localt < blwin)
			{

				post2.get();							// 

				//gsout << "post2: " << gsnl;
				//gsout << post2.values << gsnl;

				float t = 1.0f - ((blwin - localt) / blwin);	// t here varies from 0 to 1 in blending window
				interp(post1, post2, t, post3);		// 

				//gsout << "post3: " << gsnl;
				//gsout << post3.values << gsnl;

				post3.apply();							// 
			}

			/** PLOTTING OBJECT PATH **/

			plot_coord = rootj->pos()->value();

			plot_coord.y = 1;

			if (_toward_goal.len() > 50) {

				point = sqrt(gs_point_segment_dist2(plot_coord.x, plot_coord.z, _stposition.x, _stposition.z, v->_flag_point->P[id].x, v->_flag_point->P[id].z));
				_toward_goal = GsVec2(plot_coord.x, plot_coord.z) - GsVec2(v->_flag_point->P[id].x, v->_flag_point->P[id].z);
				
				//gsout << "TO GOAL: " << _toward_goal.len() << gsnl;
				//gsout << "POINT:" << point << gsnl;

				number_point++;
				sum_point += point;

			}
			else {

				id++;
				_stposition = rootj->pos()->value();

				if (id < v->_flag_point->P.size()) {

					average_dist = sum_point / number_point;
					gsout << "AVERAGE DISTANCE: " << average_dist << gsnl;

					gsout << "TIME(s): " << gs_time() - starting << gsnl;

					starting = gs_time();

					gsout << "GO TO FLAG" << id + 1 << gsnl;

					point = sqrt(gs_point_segment_dist2(plot_coord.x, plot_coord.z, _stposition.x, _stposition.z, v->_flag_point->P[id].x, v->_flag_point->P[id].z));
					_toward_goal = GsVec2(plot_coord.x, plot_coord.z) - GsVec2(v->_flag_point->P[id].x, v->_flag_point->P[id].z);

					sum_point = point;
					number_point = 1;

					v->_pole_line->init();
					v->_pole_line->line_width(3.0f);
					v->_pole_line->color(GsColor::darkgray);
					v->_pole_line->push(v->_flag_point->P[id], GsVec(v->_flag_point->P[id].x, 500, v->_flag_point->P[id].z));
					
					v->_arrow_line->push(plot_coord, v->_flag_point->P[id]);

					GsVec _goal_vec = plot_coord - v->_flag_point->P[id];
					_goal_vec.normalize();
					_goal_vec = _goal_vec * 150;

					GsQuat ang, reang, flag_ang;
					ang.set(GsVec::j, gspi / 9);
					reang.set(GsVec::j, -gspi / 9);
					flag_ang.set(GsVec::j, gspi / 2);

					v->_arrow_line->push(v->_flag_point->P[id] + ang.apply(_goal_vec), v->_flag_point->P[id]);
					v->_arrow_line->push(v->_flag_point->P[id] + reang.apply(_goal_vec), v->_flag_point->P[id]);

					v->_flag_object->model()->V[0].set(GsVec(v->_flag_point->P[id].x, 500, v->_flag_point->P[id].z));
					v->_flag_object->model()->V[1].set(GsVec(v->_flag_point->P[id].x, 400, v->_flag_point->P[id].z));
					v->_flag_object->model()->V[2].set((GsVec(v->_flag_point->P[id].x, 450, v->_flag_point->P[id].z) + flag_ang.apply(_goal_vec)));

				}
				else if (id== v->_flag_point->P.size()+1) {

					average_dist = sum_point / number_point;

					gsout << "AVERAGE DISTANCE: " << average_dist << gsnl;

					gsout << "TIME(s): " << gs_time() - starting << gsnl;

					ui_message("GAME FINISH!!!", "Messgae");

					gsout << "FINISH!!!"<< gsnl;
				}
			}
			v->_object_plot->push(plot_coord);
			v->_object_path->push(pre_coord, plot_coord);

			pre_coord = plot_coord;


			// Set camera:
			if (v->follow_but())
			{
				v->camera().center = rootj->pos()->value();

				//v->camera().eye = v->camera().center + dir_y.apply(GsVec(0, 0, -250));
				v->camera().eye = v->camera().center + GsVec(0, 0, -500);

				v->rotate_camera(dir_y);

				v->camera().center.y = root_y + 50.0f;
				v->camera().eye.y = root_y + 250.0f;

			}

			if (v->camera_check)
			{
				v->camera().center = rootj->pos()->value();
				float rx, ry, rz;
				
				gs_angles(gsYXZ, rootj->rot()->value(), rx, ry, rz);		// Exploit Quotanion for just Y, Root Global Direction
				dir_y.set(GsVec::j, ry);
				
				//v->camera().eye = v->camera().center + dir_y.apply(GsVec(0, 0, -250));
				v->camera().eye = v->camera().center + GsVec(0, 0, -500);

				v->rotate_camera(dir_y);

				v->camera().center.y = root_y + 50.0f;
				v->camera().eye.y = root_y + 250.0f;
				
			}

			v->sk_scene()->update();
			v->message().setf("Keys: w,s,t  state=%d  loct=%4.2f  dur=%4.2f", state, localt, curm->duration());
			v->render();

			ws_check(); // this updates window and interface (otherwise application will be unresponsive)
		}
	}

	return 1;
}


