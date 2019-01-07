# pragma once

# include <sig/sn_group.h>
# include <sig/sn_points.h>
# include <sig/sn_lines.h>

# include <sigogl/ui_slider.h>
# include <sigogl/ui_button.h>
# include <sigogl/ui_check_button.h>
# include <sigogl/ws_viewer.h>

# include <sigkin/kn_skeleton.h>
# include <sigkin/kn_scene.h>

# include "particle_system.h"
# include "move_graph.h"

class MyViewer : public WsViewer
 { 
   public: // scene and other data:
	SnGroup*  _root;
	//SnGroup*  _spheres;
	SnGroup*  _enemys; 
	//SnGroup*  _obstacles; 
	//SnGroup*  _walls;
	SnGroup*  _targets; 
	SnGroup*  _animation;
	//SnPoints* _points;
	//SnLines* _lines;
	SnLines* _vel_lines;
	SnLines* _world;
	//GsArray<GsMat*> _positions;
	GsArray<GsMat*> _enemy_positions;
	
	float _sphereradius;
	ParticleSystem* _psys;
	int _nfaces;
	
	GsQuat ang, reang;

	//GsArray<KnSkeleton*> _skeletons;
	GsArray<KnScene*> _knscenes;
	GsArray<float> rt_info;

	KnScene* _kns;
	MoveGraph* mg;
	KnSkeleton* sk;
	KnJoint* rootj;

	GsVec pr;

	float localt;
	double mt0;

   public: // ui:
	UiButton* _init;
	UiCheckButton* _run;
	UiCheckButton* _wind;
	UiCheckButton* _scol;
	UiCheckButton* _vcol;
	UiCheckButton* _bounce;
	UiSlider* _windslider;
	UiSlider* _tscaleslider;
	UiSlider* _crestslider;
	UiCheckButton* _vvel;
	UiCheckButton* _vworld;
	UiCheckButton* _vsphere;
	UiCheckButton* _vagt;
	UiCheckButton* _vani;
	UiCheckButton* _varr;
	UiCheckButton* _vtgt;

   public:
	MyViewer ( int x, int y, int w, int h, const char *l=0 );
   ~MyViewer ();

	void build_ui ();
	void sphere_radius ( float r ) { _sphereradius=r; }

	void view(bool bool_ani, bool bool_agn, bool bool_arr, bool bool_tgt);
	void build_scene(ParticleSystem& psys, int nfaces);
	void update_scene ();

	void run ();

   public :
	virtual int handle_scene_event ( const GsEvent &e );
	virtual int uievent ( int e );

   protected:
	virtual int handle_keyboard(const GsEvent &e) override;

};

