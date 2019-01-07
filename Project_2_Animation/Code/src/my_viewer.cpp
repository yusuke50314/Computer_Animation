
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>

# include <sigogl/ws_run.h>

# include <Windows.h>

MyViewer::MyViewer(KnSkeleton* sk, int x, int y, int w, int h, const char* l ) : WsViewer(x,y,w,h,l)	// CHECKED
{
	//gs_point_segment_dist2();
	//gsout << gs_point_segment_dist2(1, 1, 0, 0, 0, 2) << gsnl;

	_blendbut = 0;			// Check Box Initialization
	_followbut = 0;			// Check Box Initialization
	_animating = false;
	_lastkey = 0;			// Keyboard Button
	camera_check = false;
	build_ui();				// UI Setting


	_object_path = new SnLines;
	_object_path->init();
	_object_path->line_width(2.0f);
	_object_path->color(GsColor::blue);
	rootg()->add(_object_path);

	_object_plot = new SnPoints;
	_object_plot->init();
	_object_plot->point_size(3.0f);
	_object_plot->color(GsColor::blue);
	rootg()->add(_object_plot);


	/** FLAG COORDINATE **/


	_flag_point = new SnPoints;
	_flag_point->init();
	_flag_point->point_size(3.0f);
	_flag_point->color(GsColor::white);

	_flag_point->push(GsVec(0, 1, 2000));			// 0
	_flag_point->push(GsVec(2000, 1, 2000));		// 1
	_flag_point->push(GsVec(2000, 1, 4000));		// 2
	_flag_point->push(GsVec(4000, 1, 4000));		// 3
	_flag_point->push(GsVec(4000, 1, 6000));		// 4

	rootg()->add(_flag_point);

	_flag_point->visible(false);

	/** POLE **/


	_pole_line = new SnLines;
	_pole_line->init();
	_pole_line->line_width(3.0f);
	_pole_line->color(GsColor::darkgray);
	_pole_line->push(_flag_point->P[0], GsVec(_flag_point->P[0].x, 500, _flag_point->P[0].z));
	rootg()->add(_pole_line);


	/** ARROW **/

	_arrow_line = new SnLines;
	_arrow_line->init();
	_arrow_line->line_width(2.0f);
	_arrow_line->color(GsColor::white);
	//_arrow_line->line_width(3.0f);
	//_arrow_line->color(GsColor::black);

	_arrow_line->push(GsVec::null, _flag_point->P[0]);

	GsVec _goal_vec = GsVec::null - _flag_point->P[0];
	_goal_vec.normalize();
	_goal_vec = _goal_vec * 150;
	
	//gsout << _goal_vec.len() << gsnl;

	GsQuat ang, reang, flag_ang;
	ang.set(GsVec::j, gspi / 9);
	reang.set(GsVec::j, -gspi / 9);
	flag_ang.set(GsVec::j, gspi / 2);

	_arrow_line->push(_flag_point->P[0] + ang.apply(_goal_vec), _flag_point->P[0]);
	_arrow_line->push(_flag_point->P[0] + reang.apply(_goal_vec), _flag_point->P[0]);

	rootg()->add(_arrow_line);


	/** FLAG **/

	_flag_object = new SnModel;
	_flag_object->color(GsColor::red);

	_flag_object->model()->V.size(3);
	_flag_object->model()->V[0].set(GsVec(_flag_point->P[0].x, 500, _flag_point->P[0].z));
	_flag_object->model()->V[1].set(GsVec(_flag_point->P[0].x, 400, _flag_point->P[0].z));
	_flag_object->model()->V[2].set((GsVec(_flag_point->P[0].x, 450, _flag_point->P[0].z) + flag_ang.apply(_goal_vec)));

	_flag_object->model()->F.push().set(0, 1, 2);
	_flag_object->model()->F.push().set(0, 2, 1);

	rootg()->add(_flag_object);


	_kns = new KnScene;									// Scene
	_kns->connect(sk);									// Connect with Skeleton Model

	// set visualization:
	bool viewskin = false, viewskel = false, viewvisg = false, viewcolg = false, viewaxis = false;
	if (sk->skin())
	{
		viewskin = true;
	}
	else if (sk->visgeos() == 0) // if no geometries show the skeleton:
	{
		viewskel = true; viewvisg = false;
	}
	else viewvisg = true;
	_kns->set_visibility(viewskin, viewskel/*skel*/, viewvisg/*visgeo*/, viewcolg/*colgeo*/, viewaxis/*axis*/);
	rootg()->add(_kns);							    // Add to Root
	rootg()->add(_object_path);
	rootg()->add(_object_plot);

}

void MyViewer::build_ui ()	// CHECKED
{
	UiPanel *p, *sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel ( "", UiPanel::HorizLeft );
	p->add ( new UiButton ( "Options", sp=new UiPanel() ) );
	{	UiPanel* p=sp;
		p->add ( _blendbut=new UiCheckButton ( "Blend", EvBlend, true ) ); 
		p->add ( _followbut=new UiCheckButton ( "Follow", EvFollow, true ) ); 
	}
	p->add ( new UiButton ( "Exit", EvExit ) ); p->top()->separate();
}

void MyViewer::build_scene ( KnSkeleton* sk, bool addfloor ) // CHECKED
{
	if ( addfloor )
	{	// Let's add a ground floor:

		SnModel *floor = new SnModel();
		GsModel &tex = *floor->model();

		GsVec a = GsVec(-10000.0f, 0, -10000.0f);
		GsVec b = GsVec(-10000.0f, 0, 10000.0f);
		GsVec c = GsVec(10000.0f, 0, -10000.0f);
		GsVec d = GsVec(10000.0f, 0, 10000.0f);

		tex.V.push() = a;
		tex.V.push() = b;
		tex.V.push() = c;
		tex.V.push() = b;
		tex.V.push() = d;
		tex.V.push() = c;

		tex.F.push() = GsModel::Face(0, 1, 2);
		tex.F.push() = GsModel::Face(3, 4, 5);

		for (int i = 0; i < tex.V.size(); i++) {
			tex.N.push(GsVec::j);
		}

		tex.T.push() = GsVec2(0.0f, 0.0f);
		tex.T.push() = GsVec2(0.0f, 100.0f);
		tex.T.push() = GsVec2(100.0f, 0.0f);
		tex.T.push() = GsVec2(0.0f, 100.0f);
		tex.T.push() = GsVec2(100.0f, 100.0f);
		tex.T.push() = GsVec2(100.0f, 0.0f);

		GsModel::Group &g = *tex.G.push();
		g.fi = 0; 
		g.fn = tex.F.size();
		g.dmap = new GsModel::Texture; 
		GsString filename = "grass.jpg";
		g.dmap->fname.set(filename);
		tex.M.push().init();

		tex.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
		tex.textured = true;

		rootg()->add(floor);
	}

}

int MyViewer::handle_keyboard ( const GsEvent &e )
{
	//gsout << "CHECK: " << e.key << " & " << _lastkey << gsnl;


	if (e.key == _lastkey) {
		//gsout << "GO" << gsnl;
		return 0;
	}


	int on = WsViewer::handle_keyboard ( e );			// 1st let system check events

	if (on) {
		//gsout << "come" << gsnl;
		return on; 
	}

	_lastkey = e.key;

	//gsout << "Touch: " << e.key << gsnl;

	//gsout<<"Key pressed: "<<e.key<<gsnl;				// Debugging

	switch ( e.key ){
		case GsEvent::KeyEnter: on_camera(); return 1;		// Exit Application
		case GsEvent::KeyEsc : gs_exit(); return 1;		// Exit Application
	}

	return 0;
}

int MyViewer::handle_key_release(const GsEvent &e)
{
	//gsout << "Release: " << e.key << gsnl;

	int off = WsViewer::handle_key_release(e);			// 1st let system check events

	if (off) {
		//gsout << "off: " << off << gsnl;
		return off;
	}

	_lastkey = 1;

	switch (e.key) {
	case GsEvent::KeyEnter: off_camera(); return 1;		// Exit Application
	}

	return 0;
}

int MyViewer::uievent ( int e )
{
	switch ( e )
	{	case EvBlend: break;			// Blending
		case EvFollow: break;			// Following the Objsct
		case EvExit: gs_exit();			// Exit
	}
	return WsViewer::uievent(e);
}
