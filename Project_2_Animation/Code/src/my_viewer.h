# pragma once

# include <sig/sn_model.h>
# include <sigkin/kn_ik_manipulator.h>
# include <sigkin/kn_motion.h>


# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>
# include <sig/sn_points.h>
# include <sig/sn_model.h>
# include <sigogl/ui_check_button.h>
# include <sigogl/ws_viewer.h>

# include <sigkin/kn_skeleton.h>
# include <sigkin/kn_scene.h>

// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	enum MenuEv { EvBlend, EvFollow, EvExit, EvJoint = 600};
	KnScene* _kns;
	UiCheckButton* _blendbut;
	UiCheckButton* _followbut;
	bool _animating;
	int _lastkey;
	KnScene* _sksc;
	KnSkeleton* _sk;
	int _seljoint;
	GsArray<KnJoint*> _effs;
	UiPanel* _jointsp;
   public:
	bool camera_check;
	SnLines* _object_path;	
	SnPoints* _object_plot;
	SnLines* _goal_path;
	
	SnPoints* _goal_plot;

	SnPoints* _flag_point;
	SnLines* _pole_line;
	SnModel* _flag_object;

	SnLines* _arrow_line;

   public:
	MyViewer(KnSkeleton* sk, int x, int y, int w, int h, const char* l);
	void build_scene ( KnSkeleton* sk, bool addfloor );
	KnScene* sk_scene() { return _kns; }
	int lastkey () { return _lastkey; }
	void invalidate_lastkey () { _lastkey=0; }
	bool blend_but () const { return _blendbut->value(); }
	bool follow_but () const { return _followbut->value(); }
	void on_camera(){ camera_check = true;}
	void off_camera() { camera_check = false; }

   protected:
	void build_ui ();
	virtual int handle_keyboard ( const GsEvent &e ) override;
	virtual int handle_key_release(const GsEvent &e) override;
	virtual int uievent ( int e ) override;
};

