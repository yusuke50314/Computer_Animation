
# pragma once

# include <sig/gs_array.h>
# include <sig/gs_box.h>
# include <sig/gs_quat.h>
# include <sig/gs_vars.h>

//# include "particle.h"

# include "agent.h" 

class ParticleSystem
 {
   private :
	GsArray<Agent> _agents; 
	GsArray<GsVec> _targets; 
	
	GsBox _world;
	GsCharPt _lcfgfile;

	int grid;
	float max_velocity;
	int agent_case;
	int agn_num;
	int max_st;
	int max_ed;

   public :
	ParticleSystem ();
   ~ParticleSystem ();

	const char* last_cfg_file () const { return _lcfgfile; }

	void init ( const char* cfgfile );
	void init ( const GsVars* vars );

	const GsArray<Agent>& agents_info() const { return _agents; } 
	const GsArray<GsVec>& targets_info() const { return _targets; } 

	int grid_info() { return grid; }; 

	const GsBox& world() const { return _world; }

	int collide_checker(GsVec position, int agt_num);

	void steering(float deltat);
	void range_limit(GsVec original_vel, GsVec new_vel, Agent &agent, int &check, GsVec total, int agt_num);
	GsVec truncate(GsVec vector, float max);
	void seek(Agent &agent, GsVec &target, GsVec &sep, GsVec &align, GsVec &cohes, int agt_num);
	
	int find_agents(Agent &agent, GsVec ahead, GsVec ahead2, int agt_num);
	int intersect_agents(GsVec ahead, GsVec ahead2, Agent &other, Agent &agent);
};
