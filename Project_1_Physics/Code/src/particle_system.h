
# pragma once

# include <sig/gs_array.h>
# include <sig/gs_box.h>
# include <sig/gs_quat.h>
# include <sig/gs_vars.h>

# include "particle.h"

class ParticleSystem
 { 
	private : //Initialization

		// Initialization of Parameters
		float _time;						// コンストラクタ
		GsVec _gravity;						// コンストラクタ
		bool _bounce;						// コンストラクタ
		bool _collides;						// コンストラクタ
		bool _collidev;						// コンストラクタ
		float _restitution;					// コンストラクタ

		float _colradius;					// コンストラクタ
		float _bigcolradius;				// コンストラクタ

		// Spring coefficient
		float _colks_vh;					// コンストラクタ
		float _colks_cross;					// コンストラクタ
		float _colks_particle;				// コンストラクタ
		float _colks_big;					// コンストラクタ
		float _colks_floor;					// コンストラクタ

		// Damping Coefficient
		float _colkd;						// コンストラクタ

		// Clothes' row / col
		int p_row;							// init
		int p_col;							// init

		// Input File
		GsCharPt _lcfgfile;

		// Particles
		GsArray<Particle> _particles;		// init: Particle Information
		Particle _bigparticle;

		// Particles Distance
		float distance;

		// Floor
		GsBox _world;

		// Compute Forces
		GsVec _extforce;					

		// Run
		GsArray<float> _state;						
		GsArray<float> _particles_derivatives;		
	
		// Stopper
		GsArray<Particle> _compare_particles;

		// Debugging
		int output;

		// Verlet Method
		GsArray<Particle> _verlet_pre_particles;
		GsArray<Particle> _verlet_cur_particles;
		int first;
		
		// Midpoint Method
		GsArray<float> _midpoint_first_state;						

		// Heun Method
		GsArray<float> _heun_first_state;						
		GsArray<float> _heun_first_derivatives;

		// Runge-Kutta Method
		GsArray<float> _runge_first_derivatives;	
		GsArray<float> _runge_first_state;						
		GsArray<float> _runge_second_derivatives;	
		GsArray<float> _runge_third_derivatives;	
		GsArray<float> _runge_forth_derivatives;	
	
    public : // Function

		ParticleSystem ();
	   ~ParticleSystem ();

		const char* last_cfg_file () const { return _lcfgfile; }

		void init ( const char* cfgfile );
		void init ( const GsVars* vars );

		void get_state();
		void set_state();

		void bounce ( bool b ) { _bounce=b; }

		void spring_collision_prevention ( bool sc ) { _collides=sc; }
		void velocity_collision_prevention ( bool vc ) { _collidev=vc; }

		void coeff_restitution ( float r ) { _restitution=r; }
		float coeff_restitution () { return _restitution; }

		void external_force ( const GsVec& f ) { _extforce=f; }
		void collision_radius ( float r ) { _colradius=r; }

		const GsBox& world() const { return _world; }

		const GsArray<Particle>& clothes() const { return _particles; }
		const Particle& bigparticle() { return _bigparticle; }

		int row() { return p_row; }
		int col() { return p_col; }

		float part_rad() { return _colradius; }
		float big_rad() { return _bigcolradius; }
		
		// Debugging
		void debug(){ output == 0 ? output = 1 : output = 0; }
		
		// Basic Function
		void clear_forces ();
		void compute_forces ();
		void compute_derivatives ();

		// Solver
		void euler_step ( float deltat );
		void midpoint_method(float deltat);
		void verlet_method(float deltat);
		void heun_method(float deltat);
		void runge_kutta_method(float deltat);

		// Improver
		void stopper();

		void translation(GsVec trans) { _bigparticle.init(_bigparticle.m, _bigparticle.x + trans);}

};
