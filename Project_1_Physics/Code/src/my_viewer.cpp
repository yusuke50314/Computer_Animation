
# include "my_viewer.h"

# include <sig/sn_material.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>

# include <sigogl/ws_run.h>

# define CHKVAR(s) if(_vars->search(s)<0) { fltk::alert("Missing parameter [%s]!",s); exit(0); }

MyViewer::MyViewer ( int x, int y, int w, int h, const char *l ) : WsViewer ( x, y, w, h, l )
{
	//Root
	_root = WsViewer::rootg();

	//SnNode
	_world = new SnLines;

	//New SnNode
	_cpoints = new SnPoints;
	_bigsphere = new SnGroup;
	_particles = new SnGroup;
	_net = new SnLines;

	_clothes = new SnGroup;

	//Function Å® Make it Visible
	view ( true, true, true); // lines, world, spheres

	//Scene Graph
	_root->add(_world);
	//_root->add(_bigsphere);
	_root->add(_cpoints);
	_root->add(_particles);
	_root->add(_net);

	_root->add(_clothes);

	//Initialization
	_sphereradius = 1.0f;
	_bigsphereradius = 10.0f;	
	solver = 4;

}

MyViewer::~MyViewer ()
{

}

enum Events {
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
	EvEuler,
	EvMidpoint,
	EvVerlet,
	EvHeun,
	EvRunge,
	EvDebug
};

void MyViewer::build_ui ()
{
	//Initialization
	UiPanel *p, *sp, *paramsp;			// pointer
	UiManager* uim = WsWindow::uim();	// UI manager
	bool detachparms = true;			// Parameter Panel: true Å® detach, false Å® attach

	// Top Menu Bar
	p = uim->add_panel ( "", UiPanel::HorizLeft );

	// Initialization Button
	p->add ( new UiButton ( "Init", EvInit ) );		

	if (!detachparms) { 
		p->add(new UiButton("Parameters", sp = new UiPanel())); 
	} 

	// View
	paramsp = sp;
	p->add ( new UiButton ( "View", sp=new UiPanel() ) );

	// Viewîzâ∫
	{
		UiPanel* p = sp;
		p->add(_vpart = new UiCheckButton("Particles", EvView, true)); 
		p->add(_vbig = new UiCheckButton("Big Particle", EvView, true));
		p->add(_vcloth = new UiCheckButton("Cloth", EvView, true));
	}
	 
	// Exit Button
	p->add ( new UiButton ( "Exit", EvExit ) );

	// Parameter Panel
	if (!detachparms){ 
		p = paramsp; // build parameters panel now
	}else{
		p = uim->add_panel("Parameters", UiPanel::Vertical, UiPanel::Left, 0, 30);
	}

	// Run
	p->add ( _run = new UiCheckButton ( "Run", EvRun ) );

	// Debug
	p->add( _debug = new UiCheckButton("Debug", EvDebug));

	// Wind
	p->add ( _wind = new UiCheckButton ( "Wind", EvWind ) ); 

	// Spring-based Collison
	p->add ( _scol = new UiCheckButton ( "Spring-Based Collision", EvSCollision ) );
	_scol->value(true);	// Initial:ON
	//_psys->spring_collision_prevention(_scol->value());

	// Velocity Reflection Collison
	p->add ( _vcol = new UiCheckButton ( "Vel. Reflection Collision", EvVCollision ) );
	_vcol->value(true);	// Initial:ON
	//_psys->velocity_collision_prevention(_vcol->value());

	// Bounce
	p->add ( _bounce = new UiCheckButton ( "Floor", EvBounce ) ); 
	_bounce->value(true);	// Initial:ON

	// Wind Force Slider
	p->add ( new UiElement("Wind Force:") ); 
	p->top()->separate();
	p->add ( _windslider = new UiSlider("", EvWind) );
	_windslider->range(-5.0f,5.0f);		// Range

	// Time Scaling Slider
	p->add ( new UiElement("Time Scale:") ); 
	p->add ( _tscaleslider = new UiSlider("", EvTScale) );
	_tscaleslider->range(0.01f,4.0f);	// Range
	_tscaleslider->value(1.0f);			// Initial Value

	// Coefficent of Restitution Slider
	p->add ( new UiElement("Coeff. of Restitution:") ); 
	p->add ( _crestslider = new UiSlider("", EvCRestitut) );

	// Solver
	p->add(new UiElement("Solver:"));
	p->top()->separate();
	p->add(new UiRadioButton("Euler", EvEuler, (solver == 0) ? true : false));
	p->add(new UiRadioButton("Midpoint", EvMidpoint, (solver == 1) ? true : false));
	p->add(new UiRadioButton("Verlet", EvVerlet, (solver == 2) ? true : false));
	p->add(new UiRadioButton("Heun", EvHeun, (solver == 3) ? true : false));
	p->add(new UiRadioButton("Runge-Kutta", EvRunge, (solver == 4) ? true : false));

}

// View Function: visible or invisible
void MyViewer::view ( bool part, bool big, bool cloth)
{
	_world->visible ( _world->visible());
	_particles->visible ( part );
	_bigsphere->visible( big );
	_cpoints->visible (!part);
	_clothes->visible(cloth);
}


// Build Scene Function
void MyViewer::build_scene ( ParticleSystem& psys, int nfaces )
{
	// Reference: Cloth's Particles
	const GsArray<Particle>& c = psys.clothes();

	// Initialization
	int i, j, csize = c.size();
	_psys = &psys;
	_nfaces = nfaces;
	_crestslider->value ( _psys->coeff_restitution() ); //Coefficent of Restitution Slider Initial Value

	_sphereradius = _psys->part_rad();
	_bigsphereradius = _psys->big_rad();

	SnModel* sphere = new SnModel;
	sphere->model()->make_sphere ( GsPnt::null, _sphereradius, nfaces, true );	// SnModel (sphere) Detail

	SnGroup* g;
	SnTransform* t;
	SnMaterial* m;

	_particles->init();			// SnGroup Initialization
	_particles_positions(0);		// Clothes Position Initialization

	for (i = 0; i<csize; i++){

		_particles->add(g = new SnGroup); // Sngroup (_particles) Å© Sngroup (g)
		g->separator(true);				// Sngroup (g) Å© Separator: ON
		g->add(t = new SnTransform);	// Sngroup (g) Å© SnTransform (t)
		g->add(m = new SnMaterial);		// Sngroup (g) Å© SnMaterial (m)
		g->add(sphere);					// Sngroup (g) Å© SnModel (sphere)

		_particles_positions.push() = &(t->get());	// &(t->get()) = true or false
		t->get().translation(c[i].x);				// SnTransform (t) Detail
		m->material().diffuse = GsColor::random();	// SnMaterial (m) Detail

	}
	
	_cloth = new SnModel;
	_cloth->model()->V.size(csize);
	for (i = 0; i < csize; i++)
	{
		_cloth->model()->V[i].set(c[i].x);
	}

	for (j = 0; j < _psys->col() - 1; j++) {
		for (i = 0; i < _psys->col() - 1; i++) {
			_cloth->model()->F.push().set(j*_psys->col() + i, j*_psys->col() + i + 1, j*_psys->col() + i + _psys->col());
			_cloth->model()->F.push().set(j*_psys->col() + i + 1, j*_psys->col() + i + _psys->col() + 1, j*_psys->col() + i + _psys->col());
			_cloth->model()->F.push().set(j*_psys->col() + i, j*_psys->col() + i + _psys->col(), j*_psys->col() + i + 1);
			_cloth->model()->F.push().set(j*_psys->col() + i + 1, j*_psys->col() + i + _psys->col(), j*_psys->col() + i + _psys->col() + 1);
		}
	}

	_clothes->init();			// SnGroup Initialization
	_clothes->add(g = new SnGroup);
	g->add(m = new SnMaterial);		// Sngroup (g) Å© SnMaterial (m)
	g->add(_cloth);

	m->material().diffuse = GsColor::cyan;	// SnMaterial (m) Detail
	

	_cpoints->init();
	_cpoints->point_size(3.0f);
	_cpoints->color(GsColor::blue);
	for (i = 0; i < csize; i++)
	{
		_cpoints->push(c[i].x);
	}

	// Reference: Big Particle
	const Particle& b = psys.bigparticle();	

	SnModel* bigsphere = new SnModel;
	bigsphere->model()->make_sphere(b.x, _bigsphereradius, nfaces, true);	// SnModel (sphere) Detail

	// Manipulater////////////////////////////////////////
	SnManipulator* manip = new SnManipulator;
	//////////////////////////////////////////////////////

	_bigsphere->init();					// SnGroup Initialization

	manip->child(_bigsphere);

	_bigsphere->add(g = new SnGroup);	// Sngroup (_bigsphere) Å© Sngroup (g)
	g->separator(true);					// Sngroup (g) Å© Separator: ON
	g->add(t = new SnTransform);		// Sngroup (g) Å© SnTransform (t)
	g->add(m = new SnMaterial);			// Sngroup (g) Å© SnMaterial (m)
	g->add(bigsphere);					// Sngroup (g) Å© SnModel (bigsphere)

	manip_num = rootg()->size();

	_root->add(manip);

	_net->init();						// SnLines Initialization
	_net->color(GsColor::blue);			// SnLines (_net) Å© Color (blue)

	for (i = 0; i<(_psys->row()); i++){

		for (j = 0; j < (_psys->col()); j++) {

			if (j != (_psys->col()-1)) {
				_net->push(c[i * (_psys->col()) + j].x, c[i * (_psys->col()) + (j + 1)].x); // Net: ècê¸
			}

			if (i != (_psys->row()-1)) {
				_net->push(c[i * (_psys->col()) + j].x, c[(i + 1) * (_psys->col()) + j].x);	// Net: â°ê¸
			}

		}

	}

	_world->init();					// SnLines Initialization
	_world->color ( GsColor::red );	// SnLines (_world) Å© Color (red)
      
	// Initilization
	int r=3;				//6*6 World Box
	float fr = (float) r;
	float fr2 = fr+fr;

	// Reference: World
	const GsBox& w = psys.world();

	GsVec dx = GsVec::i*w.dx();		// i-vector:(1,0,0)
	GsVec dz = GsVec::k*w.dz();		// k-vector:(0,0,1)

	GsPnt a ( w.a.x*fr2, w.a.y, w.a.z*fr2 );
	for ( i=-r; i<=r; i++ ){
		_world->push ( a, a+(dx*fr2) );		// World: ècê¸
		a+=dz;
	}

	a.set ( w.a.x*fr2, w.a.y, w.a.z*fr2 );	
	for ( i=-r; i<=r; i++ ){
		_world->push ( a, a+(dz*fr2) );		// World: â°ê¸
		a+=dx;
	}

}

void MyViewer::update_scene ()
{	
	// Reference: Cloth's Particles
	const GsArray<Particle>& c = _psys->clothes();

	// Initialization
	int i, j, csize=c.size();

	if ( _particles->visible() ){
	
		for ( i = 0; i<csize; i++) {
			_particles_positions[i]->setrans(c[i].x);
		}

	}else{

		for (i = 0; i<csize; i++) {
			_cpoints->P[i] = c[i].x;
			_cpoints->touch();
		}

	}

	if (_net->visible()){

		_net->init();

		for (i = 0; i<(_psys->row()); i++) {

			for (j = 0; j < (_psys->col()); j++) {

				if (j != (_psys->col()-1)) {
					_net->push(c[i * (_psys->col()) + j].x, c[i * (_psys->col()) + (j + 1)].x);
				}

				if (i != (_psys->row()-1)) {
					_net->push(c[i * (_psys->col()) + j].x, c[(i + 1) * (_psys->col()) + j].x);
				}

			}
		}
	}

	
	if (_clothes->visible()) {
		
		for (i = 0; i < csize; i++)
		{
			_cloth->model()->V[i].set(c[i].x);
		}

	}

}

void MyViewer::run ()
{
	if ( !_run->value() ) return; // turning sim off

	double tfac = 1.0; //ui_timeslider->value();
	double t0, tprior, tnow;
	double com_st, com_end, com_time;

	int count = 0;
	double sum = 0.0;
	double ave_time = 0.0;
	double delta_sum = 0.0;
	double delta_time = 0.0;
	double delta_average = 0.0;


	SnManipulator* manip = rootg()->get<SnManipulator>(manip_num); // access one of the manipulators
	GsMat mat = manip->mat();

	_psys->translation(GsVec(mat.e14, mat.e24, mat.e34));

	t0 = tprior = gs_time()*tfac;
	while ( _run->value() )
	{
	
		// draw current state:
		update_scene ();
		ws_check();

		// check time scale:
		if ( tfac!=_tscaleslider->value() )
		{	t0 /= tfac;
			tprior /= tfac;
			tfac=_tscaleslider->value();
			t0 *= tfac;
			tprior *= tfac;
		}

		// compute next step:
		tnow = gs_time () * tfac;
		com_st = gs_time() * tfac;
		

		if (solver == 0) {
			_psys->euler_step(float(tnow - tprior));
		} else if (solver == 1) {
			_psys->midpoint_method(float(tnow - tprior));
		}
		else if (solver == 2) {
			_psys->verlet_method(float(tnow - tprior));
		}
		else if (solver == 3) {
			_psys->heun_method(float(tnow - tprior));
		}
		else if (solver == 4) {
			_psys->runge_kutta_method(float(tnow - tprior));
		}

		com_end = gs_time() * tfac;
		com_time = com_end - com_st;
		delta_time = tnow - tprior;

		count++;
		sum += com_time;
		delta_sum += delta_time;

		if (count == 3000) {
			ave_time = sum / 3000.0;
			delta_average = delta_sum / 3000.0;

			count = 0;
			sum = 0;
			delta_sum = 0;
		}

		// display some info:
		message().setf("run: %5.2f s, deltaT: %5.6f s, computation time: %5.6f s", float(tnow - t0), float(delta_average), float(ave_time));
		render();
		tprior = tnow;

	}
}

int MyViewer::uievent ( int e )
{
	switch ( e ){

	case EvInit:
		{	

		_psys->init ( _psys->last_cfg_file() );
		build_scene ( *_psys, _nfaces );


		} break;

	case EvWind:
		{	
		float windmag = _windslider->value();
		if (!_wind->value()) {
			windmag = 0;
		}
		_psys->external_force ( GsVec( windmag, 0, 0 ) );
		} break;

	case EvBounce:
		{	
		_psys->bounce ( _bounce->value() );
		_world->visible(_bounce->value());
		} break;

	case EvSCollision:
		{	
		_psys->spring_collision_prevention ( _scol->value() );
		} break;

	case EvVCollision:
		{	
		_psys->velocity_collision_prevention ( _vcol->value() );
		} break;

	case EvCRestitut:
		{	
		_psys->coeff_restitution ( _crestslider->value() );
		} break;

	case EvView:
		{	
		view ( _vpart->value(), _vbig->value(), _vcloth->value());
		} break;

	case EvEuler:
		{
		solver = 0;
		} break;

	case EvMidpoint:
		{
		solver = 1;
		} break;

	case EvVerlet:
	{
		solver = 2;
	} break;

	case EvHeun:
	{
		solver = 3;
	} break;

	case EvRunge:
	{
		solver = 4;
	} break;

	case EvRun: run(); break;

	case EvDebug:
	{
		_psys->debug();
	} break;

	case EvExit: gs_exit();

	}
	return WsViewer::uievent(e);
}

int MyViewer::handle_scene_event ( const GsEvent &e )
{
	if ( e.button1 )
	{	// nothing here for now
	}

	return WsViewer::handle_scene_event ( e );
}
