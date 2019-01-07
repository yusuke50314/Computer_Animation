
# include "particle_system.h"
# include <sig/gs_string.h>

# include <sigogl/ui_dialogs.h>

# include <sig/gs_trace.h>

//======================================== ParticleSystem ===============================================

ParticleSystem::ParticleSystem ()
{	
	// Initialization
	_time = 0;						// Time Parameter
	output = 0;						// Debugging on / off
	first = 1;						// Verlet Method Parameter

	_gravity.set(0, -9.807f, 0);	// Gravity Vector

	// Checkbox Initial Value
	_bounce = true;
	_collides = true;
	_collidev = true;
	_restitution = 0.5f;				

	_colradius = 1.0f;				// Sphere Radius
	_bigcolradius = 10.0f;			// Big Sphere Radius 

	_colks_vh = 1.0f;				// Spring Coefficent 
	_colks_cross = 1.0f;			// Spring Coefficent 
	_colks_particle = 1.0f;			// Spring Coefficent 
	_colks_big = 1.0f;				// Spring Coefficent 
	_colks_floor = 1.0f;			// Spring Coefficent 

	_colkd=1.0f;					// Damping Coefficent
	
}

ParticleSystem::~ParticleSystem ()
{
}

// Input File
void ParticleSystem::init ( const char* cfgfile )
{
	GsInput in;

	if ( !cfgfile ) cfgfile = ui_select_file ( "Enter Configuration File", 0, "*.txt" ); //Allert
	if ( !cfgfile ) gsout.fatal ( "Configuration file needed!" );						 //Allert
	if ( !in.open(cfgfile) ) gsout.fatal ( "Could not open cfg file!");					 //Allert

	_lcfgfile = cfgfile;			// Input File Name

	// Initialization
	GsVars* params = new GsVars;

	in.commentchar ( '#' );			// ÉRÉÅÉìÉg
	in.lowercase ( false );			// Change to Lower Case

	while ( true )
	{	
		if (in.get() == GsInput::End) {
			break; 
		}

		GS_TRACE1 ( "Token: "<<in.ltoken() );

		if ( in.ltoken()=="parameters" ){	
			in >> *params;
		} else if ( in.ltoken()=="end" ){
			break;
		}

	}

	GS_TRACE1("End Parsing");

	init ( params );				// ParticleSystem::init( vars )
	delete params;

}

// Initializtion
void ParticleSystem::init ( const GsVars* vars )
{
	// Parameters from the Input File
	p_row = vars->geti("particles", 0);	// Particle's Row Number
	p_col = vars->geti("particles", 1);	// Particle's Column Number
	distance = vars->getf("distance");	// Distance

	
	GsVec initpos(vars->getf("initpos", 0), vars->getf("initpos", 1), vars->getf("initpos", 2));				// Particles Position
	

	_restitution = vars->getf("restitution");			// îΩî≠åWêî

	GsVar* v;
	v = vars->get("worldbox");				// Floor
	_world.set(GsVec(v->getf(0), v->getf(1), v->getf(2)), GsVec(v->getf(3), v->getf(4), v->getf(5)));


	float mass1 = vars->getf("part.mass",0); // Minéøó 
	float mass2 = vars->getf("part.mass",1); // Maxéøó 

	float vel1 = vars->getf("part.vel", 0);	// Minë¨ìx
	float vel2 = vars->getf("part.vel", 1);	// Maxë¨ìx

	float rest1 = vars->getf("part.restit",0); 
	float rest2 = vars->getf("part.restit",1);
	
	GsVec vel ( vars->getf("part.dir",0), vars->getf("part.dir",1), vars->getf("part.dir",2) ); // Moving Direction
	GsString velrand = vars->gets("part.dir.rand");

	_colks_vh = vars->getf("col.ks.vh");				// Spring Coefficient
	_colks_cross = vars->getf("col.ks.cross");			// Spring Coefficient
	_colks_particle = vars->getf("col.ks.particle");	// Spring Coefficient
	_colks_big = vars->getf("col.ks.big");				// Spring Coefficient
	_colks_floor = vars->getf("col.ks.floor");			// Spring Coefficient

	_colkd = vars->getf("col.kd");						// Damping Coefficient

	_colradius = vars->getf("part.radius");				// Sphere Radius
	_bigcolradius = vars->getf("big.radius");			// Big Sphere Radius 
	
	

	
	
	

	if (rest1 <= 0.1f) {
		rest1 = 0.1f; // 0.1à»â∫ Å® 0.1
	}
	if (rest2 >= 1.0f) {
		rest2 = 1.0f; // 1.0à»è„  Å® 1.0
	}


	// Initialization Parameter			
	_time = 0;
	output = 0;						// Debugging on / off
	first = 1;						// Verlet Method Parameter
	int c_size = p_row * p_col;				// Clothes' Particles Number

	// Initialization Size
	_particles.size(c_size);

	// Stopper
	_compare_particles.size(c_size);

	// Basic
	_state.size(_particles.size() * 6);							// Particle's State Å® X_x/X_y/X_z/V_x/V_y/V_z 
	_particles_derivatives.size(_particles.size() * 6);			// Particle's State Å® X_x/X_y/X_z/V_x/V_y/V_z 
	
	// Verlet Method
	_verlet_pre_particles.size(c_size);
	_verlet_cur_particles.size(c_size);

	// Midpoint Method
	_midpoint_first_state.size(_state.size() * 6);

	//Heun Method
	_heun_first_state.size(_particles.size() * 6);
	_heun_first_derivatives.size(_particles.size() * 6);

	//Runge-Kutta Method
	_runge_first_derivatives.size(_particles.size() * 6);
	_runge_first_state.size(_particles.size() * 6);
	_runge_second_derivatives.size(_particles.size() * 6);
	_runge_third_derivatives.size(_particles.size() * 6);
	_runge_forth_derivatives.size(_particles.size() * 6);

	int i,j;			// For Loop
	GsQuat q;
	GsVec pos;
	GsVec bigpos;

	// Big Particle Initialization
	bigpos.set(vars->getf("bigpos", 0), vars->getf("bigpos", 1), vars->getf("bigpos", 2));	// Big Particle Location

	

	_bigparticle.init(gs_random(mass1, mass2),bigpos);	// Paricle Initialization
	_bigparticle.r = gs_random(rest1, rest2);			// Particle's îΩî≠åWêî
	_bigparticle.v = GsVec::null;						// Particle's ë¨ìx

	

	// Clothes' Particles Initialization
	float c_x = (distance * (p_col-1)) / 2;						// X Center
	float c_y = (distance * (p_row-1)) / 2;						// Y Center

	for (i = 0; i<p_row; i++){
		for (j = 0; j < p_col; j++) {

			pos.set((distance*i) - (c_x)+initpos.x, initpos.y, (distance*j) - (c_y)+initpos.z);		// Clothes' Particles Location
			_particles[(i*p_col)+j].init(gs_random(mass1, mass2), pos);	// Particle Initialization
			_particles[(i*p_col) + j].r = gs_random(rest1, rest2);		// Particle's îΩî≠åWêî
			vel.len(gs_random(vel1, vel2));								// Particle's ë¨ìx
			_particles[(i*p_col) + j].v = vel;

		}
	}
	
}

void ParticleSystem::get_state () // s Å© p
{
	
	float* c = &_state[0];

	for (int i = 0, csize = _particles.size(); i<csize; i++){
		Particle& q = _particles[i];
		*(c++) = q.x.x; 
		*(c++) = q.x.y; 
		*(c++) = q.x.z;
		*(c++) = q.v.x; 
		*(c++) = q.v.y; 
		*(c++) = q.v.z;
	}

}

void ParticleSystem::set_state () // p Å© s
{
	const float* c = &_state[0];

	for (int i = 0, csize = _particles.size(); i<csize; i++){
		Particle& q = _particles[i];
		q.x.x = *(c++); 
		q.x.y = *(c++); 
		q.x.z = *(c++);
		q.v.x = *(c++); 
		q.v.y = *(c++); 
		q.v.z = *(c++);
	}

}



void ParticleSystem::compute_derivatives ()
{
	clear_forces ();
	compute_forces ();

				

	float* c = &_particles_derivatives[0];						// Pointer

	for (int i = 0, csize = _particles.size(); i<csize; i++){

		Particle& q = _particles[i];
		*(c++) = q.v.x;				// Velocity X
		*(c++) = q.v.y;				// Velocity Y
		*(c++) = q.v.z;				// Velocity Z
		*(c++) = q.f.x / q.m;		// Acceleration X
		*(c++) = q.f.y / q.m;		// Acceleration Y
		*(c++) = q.f.z / q.m;		// Acceleration Z

	}

}


// Clear Forces Function
void ParticleSystem::clear_forces ()
{
	for (int i = 0, size = _particles.size(); i<size; i++){
		_particles[i].f = GsVec::null;
	}
}



void ParticleSystem::compute_forces ()
{
	int i, j, csize=_particles.size();

	// Basic Forces
	for (i = 0; i<csize; i++){
		
		_particles[i].f +=  (_particles[i].m*_gravity) + _extforce;	// F = F + mg + (Extra Force)
	}

	// Spring Collision
	if ( _collides ){

		float dist, r2=_colradius*2.0f; 
		GsVec l, dv, f;


		// ParticleìØémÇÃê⁄êGÉRÉäÉWÉáÉì
		for (i = 0; i < csize; i++) //å©ÇÈÇ◊Ç´particle
		{
			for (j = 0; j < csize; j++) //ëäéËÇ…Ç∑Ç◊Ç´particle
			{
				if (i == j) continue;
				dist = ::dist(_particles[i].x, _particles[j].x);
				if (dist > 0.0001f && dist < r2)
				{
					l = _particles[i].x - _particles[j].x;
					dv = _particles[i].v - _particles[j].v;
					f = l / dist * -(_colks_particle*(dist - r2) + dot(dv, l)*_colkd / dist); // spring behavior
					_particles[i].f += f;
				}
			}
		}

		// Particleä‘ÇÃSpringä÷åW
		for (i = 0; i<csize; i++){								// å©ÇƒÇ¢ÇÈParticle
			for (j = 0; j<csize; j++){							// ëäéËÇ…Ç∑ÇÈParticle
				if (i == j){									// ìØàÍÇÃParticle
					continue;									
				}
				if (i%col() == 0 && j == i - 1 || j == i - col() - 1 || j == i + col() - 1 ) {
					continue;
				}
				if (i%col() == col()-1 && j == i + 1 || j == i - col() + 1 || j == i + col() + 1 ) {
					continue;
				}

				if ( j == i - 1 || j == i+1 || j == i-col() || j == i+col()) {

					dist = ::dist(_particles[i].x, _particles[j].x);	// Particlesä‘ãóó£

					if (dist > 0.0001f) {					// è’ìÀéû

						l = _particles[i].x - _particles[j].x;			// Location Difference
						dv = _particles[i].v - _particles[j].v;			// Velocity Difference
						// Hooke's Damped Spring - f = - (Ks(mag(i)-r)+Kd((Va-Vb)ÅEl)/mag(l))l/mag(l)
						f = l / dist * -(_colks_vh*(dist - distance) + dot(dv, l)*_colkd / dist);
						_particles[i].f += f;							// F = F + (Spring Force)
					}

				}

				if (j == i - col() - 1 || j == i - col() + 1 || j == i + col() - 1 || j == i + col() + 1 ) {

					dist = ::dist(_particles[i].x, _particles[j].x);	// Particlesä‘ãóó£

					if (dist > 0.0001f) {					// è’ìÀéû

						l = _particles[i].x - _particles[j].x;			// Location Difference
						dv = _particles[i].v - _particles[j].v;			// Velocity Difference
						// Hooke's Damped Spring - f = - (Ks(mag(i)-r)+Kd((Va-Vb)ÅEl)/mag(l))l/mag(l)
						f = l / dist * -(_colks_cross*(dist - distance*(1.414f)) + dot(dv, l)*_colkd / dist);
						_particles[i].f += f;							// F = F + (Spring Force)
					}

				}

			}
		}

		// ParicleÇ∆Big Particleä‘ÇÃê⁄êGÉRÉäÉWÉáÉì
		float r3 = _colradius + _bigcolradius;
		for (i = 0; i < csize; i++){

			dist = ::dist(_particles[i].x, _bigparticle.x);

			if (dist > 0.0001f && dist < r3){				// è’ìÀéû
				l = _particles[i].x - _bigparticle.x;			// Location Difference
				dv = _particles[i].v;			// Velocity Difference
				// Hooke's Damped Spring - f = - (Ks(mag(i)-r)+Kd((Va-Vb)ÅEl)/mag(l))l/mag(l)
				f = l / dist * -(_colks_big*(dist - r3) + dot(dv, l)*_colkd / dist);
				_particles[i].f += f;							// F = F + (Spring Force)
			}
		}

	}

}


inline GsVec reflect ( const GsVec& n, const GsVec& v )
{
	GsVec nv, tv;
	nv = n * dot ( v, n );			// Normal Component ñ@ê¸
	tv = v - nv;					// Tangential Component ê⁄ê¸
	return tv-nv;
}



void ParticleSystem::stopper()
{
	
	int csize = _particles.size();
	float dist_pre, dist_cur, dist_force, dist_close_floor, dist_close_big;

	for (int i = 0; i < csize; i++) {
		

		dist_pre = ::dist(_compare_particles[i].v, GsVec::null);
		dist_cur = ::dist(_particles[i].v, GsVec::null);

		dist_force = ::dist(_particles[i].f, GsVec::null);
		
		dist_close_floor = ::dist(GsVec(0,_compare_particles[i].x.y,0),GsVec(0,_world.a.y,0));
		dist_close_big = ::dist(_compare_particles[i].x, _bigparticle.x) - _colradius - _bigcolradius;

		if (output == 1) {
			//if (i == 0) {
			//	gsout << "dist_pre: " << dist_pre << gsnl;
			//	gsout << "dist_cur: " << dist_cur << gsnl;
			//	gsout << "dist_force: " << dist_force << gsnl;
			//	gsout << "dist_close_big: " << dist_close_big << gsnl;
			//	gsout << "dist_close: " << dist_close_floor << gsnl;
			//	gsout << "extforce: " << _extforce << gsnl;
			//}
		}

		if (dist_pre < 2 && dist_cur < 2 && _extforce == 0 && dist_close_big < 1 ) {
			_particles[i] = _compare_particles[i];
		}

		if (dist_pre < 1 && dist_cur < 1 && _extforce == 0 && dist_close_floor < 0.5) {
			_particles[i] = _compare_particles[i];
		}


	}
	

}




// é¿çsÇP
void ParticleSystem::euler_step ( float deltat )
{
	int csize = _particles.size();

	for (int i = 0; i < csize; i++) {
		_compare_particles[i] = _particles[i];
	}

	compute_derivatives ();								// Velocity & Acceleration

	int i, c_stsize=_particles_derivatives.size();

	for (i = 0; i < c_stsize; i++) {
		_particles_derivatives[i] *= deltat;				//(Vdel(t), Adel(t))
	}

	get_state ();										// State (Location & Velocity)

	for (i = 0; i < c_stsize; i++) {						
		_state[i] += _particles_derivatives[i];			//(X_t+del(t)=X_t+Vdel(t), V_t+del(t)=V_t+Adel(t))
	}

	set_state ();										// State (Location & Velocity)

	_time += deltat;									// Time Accumulation


	// Velocity Collision
	if ( _collidev ){

		float dist, r2=_colradius*2.0f; 
		GsVec n, v;
		int i, j, csize=_particles.size();

		for (i = 0; i<csize; i++){								// å©ÇƒÇ¢ÇÈParticle
			for (j = 0; j<csize; j++){							// ëäéËÇ…Ç∑ÇÈParticle

				if (i == j) {									// ìØàÍÇÃParticle
					continue; 
				}

				dist = ::dist(_particles[i].x, _particles[j].x);	// Particlesä‘ãóó£

				if (dist>0.0001f && dist<r2){					// è’ìÀéû

					n = _particles[i].x - _particles[j].x;
					n.normalize();

					v = _particles[i].v;
					v.normalize();

					if (dot(n, v) < 0) {
						_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
					}

				}
			}
		}


		float r3 = _colradius+_bigcolradius;

		for (i = 0; i<csize; i++)
		{
			dist = ::dist(_particles[i].x, _bigparticle.x);
			if (dist>0.0001f && dist<r3)
			{
				n = _particles[i].x - _bigparticle.x;
				n.normalize();
				v = _particles[i].v;
				v.normalize();
				if (dot(n, v)<0)
					_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
			}
		}

	}

	if ( _bounce ){
		GsVec n = GsVec::j; // bottom plane normal pointing up
		GsVec v, nv;

		// Simple Floor Reflection
		for (i = 0; i<_particles.size(); i++){
			
			if (_particles[i].x.y - _colradius < _world.a.y  &&  dot(_particles[i].v, n) < 0) {
				_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
			}

		}

	}
	
	stopper();

}

// é¿çsÇQ
void ParticleSystem::midpoint_method(float deltat)
{
	int csize = _particles.size();

	for (int i = 0; i < csize; i++) {
		_compare_particles[i] = _particles[i];
	}

	compute_derivatives();								// Velocity & Acceleration

	int i, c_stsize = _particles_derivatives.size();
	
	for (i = 0; i < c_stsize; i++) {
		_particles_derivatives[i] *=(deltat/2);				
	}

	get_state();										// State (Location & Velocity)


	for (i = 0; i < c_stsize; i++) {
		_midpoint_first_state[i] = _state[i];
		_state[i] = _state[i] + _particles_derivatives[i];	// Half-State (Location & Velocity)
	}
	
	set_state();
	get_state();

	compute_derivatives();
	
	for (i = 0; i < c_stsize; i++) {
		_state[i] = _midpoint_first_state[i] +_particles_derivatives[i]*deltat;		//(X_t+del(t)=X_t+Vh*del(t), V_t+del(t)=V_t+Ah*del(t))
	}

	set_state();										// State (Location & Velocity)

	_time += deltat;									// Time Accumulation


	// Velocity Collision
	if (_collidev) {

		float dist, r2 = _colradius * 2.0f;
		GsVec n, v;
		int i, j, csize = _particles.size();

		for (i = 0; i < csize; i++) {								// å©ÇƒÇ¢ÇÈParticle
			for (j = 0; j < csize; j++) {							// ëäéËÇ…Ç∑ÇÈParticle

				if (i == j) {									// ìØàÍÇÃParticle
					continue;
				}

				dist = ::dist(_particles[i].x, _particles[j].x);	// Particlesä‘ãóó£

				if (dist > 0.0001f && dist < r2) {					// è’ìÀéû

					n = _particles[i].x - _particles[j].x;
					n.normalize();

					v = _particles[i].v;
					v.normalize();

					if (dot(n, v) < 0) {
						_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
					}

				}
			}
		}


		float r3 = _colradius + _bigcolradius;

		for (i = 0; i < csize; i++)
		{
			dist = ::dist(_particles[i].x, _bigparticle.x);
			if (dist > 0.0001f && dist < r3)
			{
				n = _particles[i].x - _bigparticle.x;
				n.normalize();
				v = _particles[i].v;
				v.normalize();
				if (dot(n, v) < 0)
					_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
			}
		}

	}

	if (_bounce) {
		GsVec n = GsVec::j; // bottom plane normal pointing up
		GsVec v, nv;

		// Simple Floor Reflection
		for (i = 0; i < _particles.size(); i++) {

			if (_particles[i].x.y - _colradius < _world.a.y  &&  dot(_particles[i].v, n) < 0) {
				_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
			}

		}

	}
	
	stopper();
}

// é¿çsÇR
void ParticleSystem::verlet_method(float deltat)
{
	int csize = _particles.size();

	for (int i = 0; i < csize; i++) {
		_compare_particles[i] = _particles[i];
	}

	int i, c_stsize = _particles.size();

	if (first == 1) {

		for (i = 0; i < c_stsize; i++) {
			_verlet_pre_particles[i] = _particles[i];
			_verlet_cur_particles[i] = _particles[i];
			_particles[i].v = (_verlet_cur_particles[i].x - _verlet_pre_particles[i].x) / deltat;
		}

		first = 0;

	} else {

		clear_forces();
		compute_forces();					// Forces Å© Location & Velocity

		for (i = 0; i < c_stsize; i++) {
			_particles[i].x = (2 * _verlet_cur_particles[i].x) - (_verlet_pre_particles[i].x) + (deltat*deltat) * _particles[i].f / _particles[i].m;
		}

		for (i = 0; i < c_stsize; i++) {
			_verlet_pre_particles[i] = _verlet_cur_particles[i];
			_verlet_cur_particles[i] = _particles[i];
			_particles[i].v = (_verlet_cur_particles[i].x - _verlet_pre_particles[i].x) / deltat;
		}

	}
	
	_time += deltat;									// Time Accumulation

	// Velocity Collision
	if (_collidev) {

		float dist, r2 = _colradius * 2.0f;
		GsVec n, v;
		int i, j, csize = _particles.size();
		GsVec location_vec;
		GsVec new_location;
		GsVec pre_location;

		float dist_new;
		GsVec norm_vel;

		GsVec to_center;
		float to_per;
		float center_per;
		float dist_center;
		float remainder;
		float length;

		GsVec move;

		for (i = 0; i < csize; i++) {								// å©ÇƒÇ¢ÇÈParticle
			for (j = 0; j < csize; j++) {							// ëäéËÇ…Ç∑ÇÈParticle

				if (i == j) {									// ìØàÍÇÃParticle
					continue;
				}

				dist = ::dist(_particles[i].x, _particles[j].x);	// Particlesä‘ãóó£

				if (dist > 0.0001f && dist < r2) {					// è’ìÀéû

					n = _particles[i].x - _particles[j].x;
					n.normalize();

					v = _particles[i].v;
					v.normalize();

					if (dot(n, v) < 0) {
						_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
					
						dist_center = ::dist(_particles[j].x, _verlet_pre_particles[i].x);

						to_center = _particles[j].x - _verlet_pre_particles[i].x;
						move = _particles[i].x - _verlet_pre_particles[i].x;

						move.normalize();

						to_per = dot(to_center, move);

						center_per = sqrtf(powf(dist_center, 2) - powf(to_per, 2));

						remainder = sqrtf(powf(r2, 2) - powf(center_per, 2));

						length = to_per - remainder;
						
						new_location = _verlet_pre_particles[i].x + length * move;

						dist_new = ::dist(_particles[i].v, GsVec::null);
						dist_new = dist_new * deltat;

						norm_vel = _particles[i].v;
						norm_vel.normalize();

						_verlet_pre_particles[i].x = new_location - dist_new * norm_vel;
						_verlet_cur_particles[i].x = new_location;


					}

					
					
					

				}
			}
		}


		float r3 = _colradius + _bigcolradius;
		


		for (i = 0; i < csize; i++)
		{
			dist = ::dist(_particles[i].x, _bigparticle.x);
			if (dist > 0.0001f && dist < r3)
			{
				n = _particles[i].x - _bigparticle.x;
				n.normalize();
				v = _particles[i].v;
				v.normalize();
				if (dot(n, v) < 0) {
					_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
					
					dist_center = ::dist(_bigparticle.x, _verlet_pre_particles[i].x);

					to_center = _bigparticle.x - _verlet_pre_particles[i].x;
					move = _particles[i].x - _verlet_pre_particles[i].x;

					move.normalize();

					to_per = dot(to_center,move);

					center_per = sqrtf(powf(dist_center, 2) - powf(to_per, 2));
					
					remainder = sqrtf(powf(r3, 2) - powf(center_per, 2));

					length = to_per - remainder;

					new_location = _verlet_pre_particles[i].x + length * move;

					dist_new = ::dist(_particles[i].v, GsVec::null);
					dist_new = dist_new * deltat;

					norm_vel = _particles[i].v;
					norm_vel.normalize();

					
					_verlet_pre_particles[i].x = new_location - dist_new * norm_vel;
					_verlet_cur_particles[i].x = new_location;
					
				}
				

			}
		}

	}

	if (_bounce) {
		GsVec n = GsVec::j; // bottom plane normal pointing up
		GsVec v, nv;
		GsVec pre;
		GsVec location_vec;
		GsVec new_location;
		float s;
		float dist_new;
		GsVec norm_vel;

		// Simple Floor Reflection
		for (i = 0; i < _particles.size(); i++) {

			if (_particles[i].x.y - _colradius < _world.a.y  &&  dot(_particles[i].v, n) < 0) {
				//pre = _particles[i].v;

				location_vec = _particles[i].x - _verlet_pre_particles[i].x;

				s = (_world.a.y - _verlet_pre_particles[i].x.y) / location_vec.y;

				new_location = _verlet_pre_particles[i].x + s * location_vec;
				
				_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;

				dist_new = ::dist(_particles[i].v, GsVec::null);
				dist_new = dist_new * deltat;

				norm_vel = _particles[i].v;
				norm_vel.normalize();

				_verlet_pre_particles[i].x = new_location-dist_new*norm_vel;
				_verlet_cur_particles[i].x = new_location;

			}

		}


	}

	for (int i = 0; i < _particles.size(); i++) {
		_particles[i].x = _verlet_cur_particles[i].x;
	}
	
	stopper();

}


// é¿çsÇS
void ParticleSystem::heun_method(float deltat)
{
	int csize = _particles.size();

	for (int i = 0; i < csize; i++) {
		_compare_particles[i] = _particles[i];
	}

	compute_derivatives();								// Velocity & Acceleration

	int i, c_stsize = _particles_derivatives.size();


	for (i = 0; i < c_stsize; i++) {
		_heun_first_derivatives[i] = _particles_derivatives[i];
		_particles_derivatives[i] *= deltat;				//(Vdel(t), Adel(t))
	}

	get_state();										// State (Location & Velocity)

	
	for (i = 0; i < c_stsize; i++) {
		_heun_first_state[i] = _state[i];
		_state[i] += _particles_derivatives[i];			//(X_t+del(t)=X_t+Vdel(t), V_t+del(t)=V_t+Adel(t))
	}

	set_state();										// State (Location & Velocity)

	compute_derivatives();

	for (i = 0; i < c_stsize; i++) {
		_particles_derivatives[i] = (_particles_derivatives[i] + _heun_first_derivatives[i]) *  deltat / 2;				//(Vdel(t), Adel(t))
	}

	for (i = 0; i < c_stsize; i++) {
		_heun_first_state[i] += _particles_derivatives[i];			//(X_t+del(t)=X_t+Vdel(t), V_t+del(t)=V_t+Adel(t))
		_state[i] = _heun_first_state[i];
	}

	set_state();

	_time += deltat;									// Time Accumulation


	// Velocity Collision
	if (_collidev) {

		float dist, r2 = _colradius * 2.0f;
		GsVec n, v;
		int i, j, csize = _particles.size();

		for (i = 0; i < csize; i++) {								// å©ÇƒÇ¢ÇÈParticle
			for (j = 0; j < csize; j++) {							// ëäéËÇ…Ç∑ÇÈParticle

				if (i == j) {									// ìØàÍÇÃParticle
					continue;
				}

				dist = ::dist(_particles[i].x, _particles[j].x);	// Particlesä‘ãóó£

				if (dist > 0.0001f && dist < r2) {					// è’ìÀéû

					n = _particles[i].x - _particles[j].x;
					n.normalize();

					v = _particles[i].v;
					v.normalize();

					if (dot(n, v) < 0) {
						_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
					}

				}
			}
		}


		float r3 = _colradius + _bigcolradius;

		for (i = 0; i < csize; i++)
		{
			dist = ::dist(_particles[i].x, _bigparticle.x);
			if (dist < r3)
			{
				n = _particles[i].x - _bigparticle.x;
				n.normalize();
				v = _particles[i].v;
				v.normalize();
				if (dot(n, v) < 0)
					_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
			}
		}

	}

	if (_bounce) {
		GsVec n = GsVec::j; // bottom plane normal pointing up
		GsVec v, nv;

		// Simple Floor Reflection
		for (i = 0; i < _particles.size(); i++) {

			if (_particles[i].x.y - _colradius < _world.a.y  &&  dot(_particles[i].v, n) < 0) {
				_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
			}

		}

	}
	stopper();
}


// é¿çsÇT
void ParticleSystem::runge_kutta_method(float deltat)
{
	int csize = _particles.size();

	for (int i = 0; i < csize; i++) {
		_compare_particles[i] = _particles[i];
	}


	compute_derivatives();								// Velocity & Acceleration

	int i, c_stsize = _particles_derivatives.size();

	for (i = 0; i < c_stsize; i++) {
		_runge_first_derivatives[i] = _particles_derivatives[i];
	}

	get_state();										// State (Location & Velocity)

	for (i = 0; i < c_stsize; i++) {
		_runge_first_state[i] = _state[i];
		_state[i] = _runge_first_state[i] + _runge_first_derivatives[i] *(deltat/2);			//(X_t+del(t)=X_t+Vdel(t), V_t+del(t)=V_t+Adel(t))
	}

	set_state();										// State (Location & Velocity)

	compute_derivatives();

	for (i = 0; i < c_stsize; i++) {
		_runge_second_derivatives[i] = _particles_derivatives[i];				//(Vdel(t), Adel(t))
	}

	for (i = 0; i < c_stsize; i++) {
		_state[i] = _runge_first_state[i]+_runge_second_derivatives[i] * (deltat / 2);
	}

	set_state();

	compute_derivatives();

	for (i = 0; i < c_stsize; i++) {
		_runge_third_derivatives[i] = _particles_derivatives[i];				//(Vdel(t), Adel(t))
	}

	for (i = 0; i < c_stsize; i++) {
		_state[i] = _runge_first_state[i] + _runge_third_derivatives[i] * deltat;
	}

	set_state();

	compute_derivatives();

	for (i = 0; i < c_stsize; i++) {
		_runge_forth_derivatives[i] = _particles_derivatives[i];				//(Vdel(t), Adel(t))
	}

	for (i = 0; i < c_stsize; i++) {
		_state[i] = _runge_first_state[i] + (_runge_first_derivatives[i] + (2 * _runge_second_derivatives[i]) + (2 * _runge_third_derivatives[i]) + _runge_forth_derivatives[i]) * (deltat / 6);
	}

	_time += deltat;									// Time Accumulation


	// Velocity Collision
	if (_collidev) {

		float dist, r2 = _colradius * 2.0f;
		GsVec n, v;
		int i, j, csize = _particles.size();

		for (i = 0; i < csize; i++) {								// å©ÇƒÇ¢ÇÈParticle
			for (j = 0; j < csize; j++) {							// ëäéËÇ…Ç∑ÇÈParticle

				if (i == j) {									// ìØàÍÇÃParticle
					continue;
				}

				dist = ::dist(_particles[i].x, _particles[j].x);	// Particlesä‘ãóó£

				if (dist > 0.0001f && dist < r2) {					// è’ìÀéû

					n = _particles[i].x - _particles[j].x;
					n.normalize();

					v = _particles[i].v;
					v.normalize();

					if (dot(n, v) < 0) {
						_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
					}

				}
			}
		}


		float r3 = _colradius + _bigcolradius;

		for (i = 0; i < csize; i++)
		{
			dist = ::dist(_particles[i].x, _bigparticle.x);
			if (dist < r3)
			{
				n = _particles[i].x - _bigparticle.x;
				n.normalize();
				v = _particles[i].v;
				v.normalize();
				if (dot(n, v) < 0)
					_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
			}
		}

	}

	if (_bounce) {
		GsVec n = GsVec::j; // bottom plane normal pointing up
		GsVec v, nv;

		// Simple Floor Reflection
		for (i = 0; i < _particles.size(); i++) {

			if (_particles[i].x.y - _colradius < _world.a.y  &&  dot(_particles[i].v, n) < 0) {
				_particles[i].v = reflect(n, _particles[i].v) * _restitution * _particles[i].r;
			}

		}

	}

	stopper();
}

//======================================== EOF ===============================================
