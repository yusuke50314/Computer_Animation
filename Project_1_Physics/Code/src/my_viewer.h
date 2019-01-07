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
		SnGroup*  _root;			//�R���X�g���N�^

		//SnNode
		SnLines* _world;			//�R���X�g���N�^
	
		//New SnNode
		SnPoints* _cpoints;			//�R���X�g���N�^
		SnGroup* _bigsphere;		//�R���X�g���N�^
		SnGroup* _particles;		//�R���X�g���N�^
		SnLines* _net;				//�R���X�g���N�^
		SnGroup* _clothes;		//�R���X�g���N�^

		float _sphereradius;		//�R���X�g���N�^
		float _bigsphereradius;		//�R���X�g���N�^

		ParticleSystem* _psys;		//build_scene
		int _nfaces;				//build_scene
		SnGroup*  _spheres;			//build_scene
		
		GsArray<GsMat*> _particles_positions; //build_scene

		int solver;

		int manip_num;

		SnModel* _cloth;

	public: // UI:

		UiButton* _init;			//�{�^��
		UiCheckButton* _run;		//�`�F�b�N�{�^��
		UiCheckButton* _debug;		//�`�F�b�N�{�^��
		UiCheckButton* _wind;		//�`�F�b�N�{�^��
		UiCheckButton* _scol;		//�`�F�b�N�{�^��
		UiCheckButton* _vcol;		//�`�F�b�N�{�^��
		UiCheckButton* _bounce;		//�`�F�b�N�{�^��
		UiSlider* _windslider;		//�X���C�_�[
		UiSlider* _tscaleslider;	//�X���C�_�[
		UiSlider* _crestslider;		//�X���C�_�[

		UiCheckButton* _vpart;		//�`�F�b�N�{�^��
		UiCheckButton* _vbig;		//�`�F�b�N�{�^��
		UiCheckButton* _vcloth;		//�`�F�b�N�{�^��

	public: // Function:

		MyViewer ( int x, int y, int w, int h, const char *l=0 );	//�R���X�g���N�^
	    ~MyViewer ();												//�f�X�g���N�^

		void build_ui ();											//UI�쐬
		void view(bool part, bool big, bool cloth);				//visible or invisible

		void build_scene(ParticleSystem& psys, int nfaces);			//�V�[���쐬
		void update_scene();										//�V�[���X�V
		void run();													//���s

		void sphere_radius ( float r ) { _sphereradius=r; }

	public : // Virtual Function:
		virtual int uievent(int e);									//UI�C�x���g
		virtual int handle_scene_event ( const GsEvent &e );		//�L�[�{�[�h


};

