# pragma once

# include <sig/sn_group.h>
# include <sig/sn_points.h>
# include <sig/sn_lines.h>
# include <sig/sn_model.h>

# include <sigogl/ui_slider.h>
# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sigogl/ui_check_button.h>
# include <sigogl/ws_viewer.h>

# include "particle_system.h"

class MyViewer : public WsViewer{

	public: // Scene and Other Data:

		//Root
		SnGroup*  _root;			//コンストラクタ

		//SnNode
		SnLines* _world;			//コンストラクタ
	
		//New SnNode
		SnPoints* _cpoints;			//コンストラクタ
		SnGroup* _bigsphere;		//コンストラクタ
		SnGroup* _particles;		//コンストラクタ
		SnLines* _net;				//コンストラクタ
		SnGroup* _clothes;		//コンストラクタ

		float _sphereradius;		//コンストラクタ
		float _bigsphereradius;		//コンストラクタ

		ParticleSystem* _psys;		//build_scene
		int _nfaces;				//build_scene
		SnGroup*  _spheres;			//build_scene
		
		GsArray<GsMat*> _particles_positions; //build_scene

		int solver;

		int manip_num;

		SnModel* _cloth;

	public: // UI:

		UiButton* _init;			//ボタン
		UiCheckButton* _run;		//チェックボタン
		UiCheckButton* _debug;		//チェックボタン
		UiCheckButton* _wind;		//チェックボタン
		UiCheckButton* _scol;		//チェックボタン
		UiCheckButton* _vcol;		//チェックボタン
		UiCheckButton* _bounce;		//チェックボタン
		UiSlider* _windslider;		//スライダー
		UiSlider* _tscaleslider;	//スライダー
		UiSlider* _crestslider;		//スライダー

		UiCheckButton* _vpart;		//チェックボタン
		UiCheckButton* _vbig;		//チェックボタン
		UiCheckButton* _vcloth;		//チェックボタン

	public: // Function:

		MyViewer ( int x, int y, int w, int h, const char *l=0 );	//コンストラクタ
	    ~MyViewer ();												//デストラクタ

		void build_ui ();											//UI作成
		void view(bool part, bool big, bool cloth);				//visible or invisible

		void build_scene(ParticleSystem& psys, int nfaces);			//シーン作成
		void update_scene();										//シーン更新
		void run();													//実行

		void sphere_radius ( float r ) { _sphereradius=r; }

	public : // Virtual Function:
		virtual int uievent(int e);									//UIイベント
		virtual int handle_scene_event ( const GsEvent &e );		//キーボード


};

