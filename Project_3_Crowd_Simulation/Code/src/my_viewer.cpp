
# include "my_viewer.h"

# include <sig/sn_model.h>
# include <sig/sn_material.h>
# include <sig/sn_transform.h>

# include <sigogl/ws_run.h>

# define CHKVAR(s) if(_vars->search(s)<0) { fltk::alert("Missing parameter [%s]!",s); exit(0); }

MyViewer::MyViewer ( int x, int y, int w, int h, const char *l ) : WsViewer ( x, y, w, h, l )
{
	_root = WsViewer::rootg();

	_world = new SnLines;
	_enemys = new SnGroup;
	_targets = new SnGroup;
	_animation = new SnGroup;
	_vel_lines = new SnLines;

	view(true, true, true, true); // lines, world, spheres

	_root->add(_world);
	_root->add(_enemys);
	_root->add(_targets);
	_root->add(_vel_lines);
	_root->add(_animation);

	_sphereradius =25.0f;
	ang.set(GsVec::k, gspi / 9);
	reang.set(GsVec::k, -gspi / 9);
}

MyViewer::~MyViewer ()
{
}

enum Events 
{
	EvInit,
	EvViewSpheres, 
	EvViewVelVecs,
	EvRun, 
	EvWind, 
	EvBounce,
	EvSCollision, 
	EvVCollision, 
	EvTScale, 
	EvCRestitut,
	EvView, 
	EvExit,
	EvViewAgent, 
	EvViewAnimation, 
	EvViewArrow, 
	EvViewTarget
};

void MyViewer::build_ui ()
{
	UiPanel *p, *sp, *paramsp;
	UiManager* uim = WsWindow::uim();
	bool detachparms = true;

	// Top Menu Bar:
	p = uim->add_panel ( "", UiPanel::HorizLeft );
	p->add ( new UiButton ( "Init", EvInit ) );
	if (!detachparms) p->add ( new UiButton ( "Parameters", sp=new UiPanel() ) ); paramsp=sp;

	p->add ( new UiButton ( "Exit", EvExit ) );

	if (!detachparms) p = paramsp; // build parameters panel now
	else p = uim->add_panel ( "MENU:", UiPanel::Vertical, UiPanel::Left, 0, 30 );

	p->add ( _run = new UiCheckButton ( "Run", EvRun ) );
	p->add(_vagt = new UiCheckButton("Agents", EvViewAgent, true)); p->top()->separate();
	p->add(_vani = new UiCheckButton("Animation", EvViewAnimation, true));
	p->add(_varr = new UiCheckButton("Arrows", EvViewArrow, true));
	p->add(_vtgt = new UiCheckButton("Target", EvViewTarget, true));

}

void MyViewer::view(bool bool_agn, bool bool_ani, bool bool_arr, bool bool_tgt)
{
	_animation->visible(bool_ani);
	_enemys->visible(bool_agn);
	_vel_lines->visible(bool_arr);
	_targets->visible(bool_tgt);
}

void MyViewer::build_scene(ParticleSystem& psys, int nfaces)
{
	const GsArray<Agent>& agents = psys.agents_info(); 
	const GsArray<GsVec>& targets = psys.targets_info();

	int i;
	int _agent_size = agents.size(); 

	_psys = &psys;
	_nfaces = nfaces;

	SnModel* sphere = new SnModel;
	sphere->model()->make_sphere(GsPnt(0, 0, 5), _sphereradius, nfaces, true);

	SnModel* cylinder = new SnModel;
	cylinder->model()->make_cylinder(GsPnt(0, 0, 0), GsPnt(0, 0, 50), _sphereradius, _sphereradius, nfaces, true);

	SnModel* circle_edge = new SnModel;
	circle_edge->model()->make_cylinder(GsPnt(0, 0, 0), GsPnt(0, 0, 5), _sphereradius, _sphereradius, nfaces, true);

	SnModel* circle = new SnModel;
	circle->model()->make_cylinder(GsPnt(0.0f, 0.0f, -2.5f), GsPnt(0, 0, 0), 25.0f, 25.0f, nfaces, true);

	SnGroup* g;
	SnTransform* t;
	SnMaterial* m;

	_enemys->init();
	_enemy_positions.size(0);

	for (i = 0; i < _agent_size; i++)
	{
		_enemys->add(g = new SnGroup);
		g->separator(true); // transformations inside group do not affect outside group
		g->add(t = new SnTransform);
		g->add(m = new SnMaterial);
		g->add(circle_edge);
		_enemy_positions.push() = &(t->get());
		t->get().translation(agents[i].x);
		m->material().diffuse = GsColor::blue;
	}


	_targets->init();

	for (i = 0; i < _agent_size; i++)
	{
		_targets->add(g = new SnGroup);
		g->separator(true); // transformations inside group do not affect outside group
		g->add(t = new SnTransform);
		g->add(m = new SnMaterial);
		g->add(circle);
		t->get().translation(targets[i]);
		m->material().diffuse = GsColor::orange;
	}
	
	_world->init();
	_world->color(GsColor::red);

	int r = _psys->grid_info();					
	float fr = (float)r;
	float fr2 = fr + fr;
	const GsBox& w = psys.world();
	GsVec dx = GsVec::i*w.dx() / 2;
	GsVec dy = GsVec::j*w.dy() / 2;
	GsPnt a(w.a.x*fr, w.a.y *fr, w.a.z); 

	for (i = -r; i <= r; i++)
	{
		_world->push(a, a + (dx*fr2));		// Row Lines
		a += dy;

	}
	a.set(w.a.x*fr, w.a.y*fr, w.a.z);
	for (i = -r; i <= r; i++)
	{
		_world->push(a, a + (dy*fr2));		// Column Lines
		a += dx;
	}

	_vel_lines->init();
	_vel_lines->line_width(2.0f);
	_vel_lines->color(GsColor::blue);
	for (i = 0; i < _agent_size; i++)
	{
		GsVec vel;
		vel = agents[i].rt;
		vel.normalize();
		_vel_lines->push(agents[i].x, agents[i].x + 50.0f*vel);
		_vel_lines->push(agents[i].x + 50.0f*vel + ang.apply(-10.0f*vel), agents[i].x + 50.0f*vel);
		_vel_lines->push(agents[i].x + 50.0f*vel + reang.apply(-10.0f*vel), agents[i].x + 50.0f*vel);

	}

	localt = 0;
	mt0 = 0;

	_knscenes.size(0);
	rt_info.size(0);

	sk = new KnSkeleton();
	if (!sk->load("../data/hdman.s")) gsout.fatal("Error loading skeleton!");
	rootj = sk->joint("Hips");
	if (!rootj) gsout.fatal("Could not find root joint!");

	pr = rootj->pos()->value();

	GsQuat ql = rootj->rot()->value();
	GsQuat qg;
	qg.set(GsVec::i, float(GS_PI / 2.0));
	rootj->rot()->value(qg * ql);

	mg = new MoveGraph;
	mg->init(sk); 

	for (i = 0; i < _agent_size; i++) 
	{
		KnScene*_kn = new KnScene;
		_kn->connect(sk);
		_kn->set_visibility(0, 0, 1, 0, 0);
		_knscenes.push(_kn);
		_animation->add(_knscenes[i]);
		rt_info.push(0.0f);
	}

	KnMotion* curm = mg->nodes[0]->motion;
	float mduration = curm->duration();
	double mt0 = gs_time(); 

}

void MyViewer::update_scene ()
{
	const GsArray<Agent>& agents = _psys->agents_info(); /** NEW ENTRY **/
	int i; 
	int _agent_size = agents.size();

	for (i = 0; i < _agent_size; i++)
	{
		_enemy_positions[i]->setrans(agents[i].x);
	}

	_vel_lines->init();
	for (i = 0; i < _agent_size; i++)
	{
		GsVec vel;
		vel = agents[i].rt;
		vel.normalize();
		_vel_lines->push(agents[i].x, agents[i].x + 50.0f*vel);
		_vel_lines->push(agents[i].x + 50.0f*vel + ang.apply(-10.0f*vel), agents[i].x + 50.0f*vel);
		_vel_lines->push(agents[i].x + 50.0f*vel + reang.apply(-10.0f*vel), agents[i].x + 50.0f*vel);

	}

	KnMotion* curm = mg->nodes[0]->motion;
	float mduration = curm->duration();

	if (mt0 == 0) {
		mt0 = gs_time();
	}

	localt = float(gs_time() - mt0);

	if (localt > mduration) 
	{
		mt0 = mt0 + mduration;
		localt = 0;
	}

	curm->apply(localt);
	GsVec pl = rootj->pos()->value();
	GsQuat ql = rootj->rot()->value();
	GsQuat qg;
	GsVec offset = GsVec(-0.196171 + 4.0, 207.375 - 41.4375 - 12.0, -1.77214 + 10.0);
	qg.set(GsVec::i, float(GS_PI / 2.0));

	for (i = 0; i < _agent_size; i++) 
	{
		GsVec vel;
		vel = agents[i].rt;
		vel.normalize();

		GsVec2 zero = GsVec2(1, 0);
		GsVec2 rt_dir = GsVec2(vel.x, vel.y);

		float radian = oriangle(rt_dir, zero);
		GsQuat q_agent;
		q_agent.set(GsVec::j, -radian + float(GS_PI/2.0));
		
		rootj->pos()->value(qg.apply(pl - offset)+ agents[i].x);
		rootj->quat()->value(qg * q_agent);
		_knscenes[i]->update();
	}

	render();
	ws_check(); // this updates window and interface (otherwise application will be unresponsive)

}

void MyViewer::run ()
{
	if ( !_run->value() ) return; 

	double t0, tprior, tnow;

	t0 = tprior = gs_time();
	while ( _run->value() )
	{
		// draw current state:
		update_scene ();
		ws_check();

		// compute next step:
		tnow = gs_time();
		_psys->steering(float(tnow - tprior));
		tprior = tnow;

		// display some info:
		message().setf ( "run: %5.2f s", float(tnow-t0) );
	}
}

int MyViewer::uievent ( int e )
{
	switch ( e )
	{	
		case EvInit:
		{	
			_psys->init(_psys->last_cfg_file());
			for (int i = 0; i < _knscenes.size(); i++)
			{
				_knscenes[i]->init();
			}
			build_scene(*_psys, _nfaces);
		} break;

		case EvViewAgent:
		{	
			view(_vagt->value(), _vani->value(), _varr->value(), _vtgt->value());
		} break;

		case EvViewAnimation:
		{	
			view(_vagt->value(), _vani->value(), _varr->value(), _vtgt->value());
		} break;

		case EvViewArrow:
		{	
			view(_vagt->value(), _vani->value(), _varr->value(), _vtgt->value());
		} break;

		case EvViewTarget:
		{	
			view(_vagt->value(), _vani->value(), _varr->value(), _vtgt->value());
		} break;

		case EvRun: run(); break;

		case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}

int MyViewer::handle_keyboard(const GsEvent &e)
{
	int ret = WsViewer::handle_keyboard(e); // 1st let system check events
	if (ret) return ret;
	
	switch (e.key) 
	{
		case GsEvent::KeyEsc: gs_exit(); return 1;		// Exit Application
	}

	return 0;
}

int MyViewer::handle_scene_event ( const GsEvent &e )
{
	if ( e.button1 )
	{	// nothing here for now
	}

	return WsViewer::handle_scene_event ( e );
}
