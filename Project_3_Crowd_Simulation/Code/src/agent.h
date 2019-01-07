
# pragma once

/** 3 Division Vector **/
# include <sig/gs_vec.h>

class Agent
{
public:
	float m;  // mass
	GsVec x;  // position
	GsVec v;  // velocity
	GsVec f;  // force accumulator
	GsVec rt;  // root direction
	GsVec ov;
	GsVec nv;
	int s;
	int c;
	GsVec total;
	int hit;
	int st_counter;
	int ed1_counter;
	int ed2_counter;
	int goal;
	

public:
	void init(float mass, const GsVec& pos, const GsVec& vel, const GsVec& root)
	{
		m = mass;
		x = pos;
		v = vel;
		f = GsVec::null;
		rt = root;
		ov = GsVec::null; 
		nv = GsVec::null;
		s = 0; c = 0;
		total = GsVec::null;
		hit = -1;
		st_counter = 0;
		ed1_counter = 0;
		ed2_counter = 0;
		goal = 0;
	}
};
