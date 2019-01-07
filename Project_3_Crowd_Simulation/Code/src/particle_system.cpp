
# include "particle_system.h"
# include <sig/gs_string.h>

# include <sigogl/ui_dialogs.h>

//# define GS_USE_TRACE1  // keyword tracing
# include <sig/gs_trace.h>

//======================================== ParticleSystem ===============================================

ParticleSystem::ParticleSystem()
{
	agent_case = 3;
	if (agent_case == 0) { // Align
		max_velocity = 0.1f;
		max_st = 10;
		max_ed = 20;
	}
	else if (agent_case == 1) { // CIRcLE
		max_velocity = 0.2f;
		max_st = 10;
		max_ed = 20;
	}
	else if (agent_case == 2) { // 4 GROUP
		max_velocity = 0.2f;
		max_st = 10;
		max_ed = 20;
	}
	else if (agent_case == 3) { // Simple Counter
		max_velocity = 0.1f;
		max_st = 1;
		max_ed = 10;
	}
	else {
		max_velocity = 0.1f;
		max_st = 10;
		max_ed = 20;
	}
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::init(const char* cfgfile)
{


	GsInput in;
	if (!cfgfile) cfgfile = ui_select_file("Enter Configuration File", 0, "*.txt");
	if (!cfgfile) gsout.fatal("Configuration file needed!");
	if (!in.open(cfgfile)) gsout.fatal("Could not open cfg file!");
	_lcfgfile = cfgfile;

	GsVars* params = new GsVars;

	in.commentchar('#');
	in.lowercase(false);
	while (true)
	{
		if (in.get() == GsInput::End) break;
		GS_TRACE1("Token: " << in.ltoken());

		if (in.ltoken() == "parameters")
		{
			in >> *params;
		}
		else if (in.ltoken() == "end")
		{
			break;
		}
	}
	GS_TRACE1("End Parsing");

	init(params);
	delete params;
}

void ParticleSystem::init(const GsVars* vars)
{
	gsout << "=== OUTPUT ===" << gsnl;

	int i;
	grid = vars->geti("grid", 0);
	//int size = vars->geti("particles",0);

	// Agent Number
	//int agn_num = vars->geti("agn_num", 0); 

	if (agent_case == 0) {
		agn_num = 3;
	}
	else if (agent_case == 1) {
		agn_num = 12;
	}
	else if (agent_case == 2) {
		agn_num = 16;
	}
	else if (agent_case == 3) {
		agn_num = 2;
	}
	else if (agent_case == 4) {
		agn_num = 1;
	}
	else if (agent_case == 5) {
		agn_num = 8;
	}

	_agents.size(agn_num);
	_targets.size(agn_num);

	// Agent Position Info
	GsArray<int> agn_pos;
	agn_pos.size(agn_num);

	// Input File Å® Agent Info
	for (i = 0; i < agn_num; i++) {
		agn_pos[i] = vars->geti("agn_pos", i); /** NEW ENTRY **/
	}

	// Agent Info Å®Å@Agent Position
	GsArray<GsVec> enemy_pos;
	enemy_pos.size(agn_num);

	if (agent_case == 0)
	{
		enemy_pos[0].set(5 * GsVec(40.0f, 10.0f, 0.0f));
		enemy_pos[1].set(5 * GsVec(40.0f, 0.0f, 0.0f));
		enemy_pos[2].set(5 * GsVec(40.0f, -10.0f, 0.0f));
	}
	else if (agent_case == 1)
	{
		for (i = 0; i < agn_num; i++)
		{
			float pos_x = float(50 * cos(i * 30 * GS_PI / 180));
			float pos_y = float(50 * sin(i * 30 * GS_PI / 180));
			enemy_pos[i].set(5 * GsVec(pos_x, pos_y, 0.0f));
		}
	}
	else if (agent_case == 2)
	{
		enemy_pos[0].set(5 * GsVec(-55.0f, -55.0f, 0.0f));
		enemy_pos[1].set(5 * GsVec(-25.0f, -55.0f, 0.0f));
		enemy_pos[2].set(5 * GsVec(25.0f, -55.0f, 0.0f));
		enemy_pos[3].set(5 * GsVec(55.0f, -55.0f, 0.0f));
		enemy_pos[4].set(5 * GsVec(-55.0f, -25.0f, 0.0f));
		enemy_pos[5].set(5 * GsVec(-25.0f, -25.0f, 0.0f));
		enemy_pos[6].set(5 * GsVec(25.0f, -25.0f, 0.0f));
		enemy_pos[7].set(5 * GsVec(55.0f, -25.0f, 0.0f));
		enemy_pos[8].set(5 * GsVec(-55.0f, 25.0f, 0.0f));
		enemy_pos[9].set(5 * GsVec(-25.0f, 25.0f, 0.0f));
		enemy_pos[10].set(5 * GsVec(25.0f, 25.0f, 0.0f));
		enemy_pos[11].set(5 * GsVec(55.0f, 25.0f, 0.0f));
		enemy_pos[12].set(5 * GsVec(-55.0f, 55.0f, 0.0f));
		enemy_pos[13].set(5 * GsVec(-25.0f, 55.0f, 0.0f));
		enemy_pos[14].set(5 * GsVec(25.0f, 55.0f, 0.0f));
		enemy_pos[15].set(5 * GsVec(55.0f, 55.0f, 0.0f));
	}
	else if (agent_case == 3)
	{
		enemy_pos[0].set(5 * GsVec(-25.0f, 5.0f, 0.0f));
		enemy_pos[1].set(5 * GsVec(25.0f, 5.0f, 0.0f));
	}
	else if (agent_case == 4)
	{
		enemy_pos[0].set(5 * GsVec(25.0f, -10.0f, 0.0f));
	}
	else if (agent_case == 5)
	{
		enemy_pos[0].set(5 * GsVec(-55.0f, -55.0f, 0.0f));
		enemy_pos[1].set(5 * GsVec(-25.0f, -55.0f, 0.0f));
		enemy_pos[2].set(5 * GsVec(-55.0f, -25.0f, 0.0f));
		enemy_pos[3].set(5 * GsVec(-25.0f, -25.0f, 0.0f));
		enemy_pos[4].set(5 * GsVec(25.0f, 25.0f, 0.0f));
		enemy_pos[5].set(5 * GsVec(55.0f, 25.0f, 0.0f));
		enemy_pos[6].set(5 * GsVec(25.0f, 55.0f, 0.0f));
		enemy_pos[7].set(5 * GsVec(55.0f, 55.0f, 0.0f));
	}

	if (agent_case == 0)
	{
		_targets[0] = 5 * GsVec(-40.0f, 12.5f, 0.0f);
		_targets[1] = 5 * GsVec(-40.0f, 0.0f, 0.0f);
		_targets[2] = 5 * GsVec(-40.0f, -12.5f, 0.0f);
	}
	else if (agent_case == 1)
	{
		for (i = 0; i < agn_num; i++)
		{
			float pos_x = float(50 * cos((10 + (i + 6) * 30) * GS_PI / 180));
			float pos_y = float(50 * sin((10 + (i + 6) * 30) * GS_PI / 180));
			_targets[i] = 5 * GsVec(pos_x, pos_y, 0.0f);
		}
	}
	else if (agent_case == 2)
	{
		_targets[0] = enemy_pos[15];
		_targets[1] = enemy_pos[14];
		_targets[2] = enemy_pos[13];
		_targets[3] = enemy_pos[12];
		_targets[4] = enemy_pos[11];
		_targets[5] = enemy_pos[10];
		_targets[6] = enemy_pos[9];
		_targets[7] = enemy_pos[8];
		_targets[8] = enemy_pos[7];
		_targets[9] = enemy_pos[6];
		_targets[10] = enemy_pos[5];
		_targets[11] = enemy_pos[4];
		_targets[12] = enemy_pos[3];
		_targets[13] = enemy_pos[2];
		_targets[14] = enemy_pos[1];
		_targets[15] = enemy_pos[0];
	}
	else if (agent_case == 3)
	{
		_targets[0] = enemy_pos[1];
		_targets[1] = enemy_pos[0];
	}
	else if (agent_case == 4)
	{
		_targets[0].set(5 * GsVec(-25.0f, 10.0f, 0.0f));

	}
	else if (agent_case == 5)
	{
		_targets[0] = enemy_pos[7];
		_targets[1] = enemy_pos[6];
		_targets[2] = enemy_pos[5];
		_targets[3] = enemy_pos[4];
		_targets[4] = enemy_pos[3];
		_targets[5] = enemy_pos[2];
		_targets[6] = enemy_pos[1];
		_targets[7] = enemy_pos[0];
	}

	for (i = 0; i < agn_num; i++)
	{
		GsVec dir = _targets[i] - enemy_pos[i];
		dir.normalize();
		dir = dir * max_velocity;
		_agents[i].init(1.0f, enemy_pos[i], dir, dir);
	}

	// Grid World Info
	GsVar* v;
	v = vars->get("worldbox");
	_world.set(GsVec(v->getf(0), v->getf(1), v->getf(2)), GsVec(v->getf(3), v->getf(4), v->getf(5)));
}

void ParticleSystem::range_limit(GsVec original_vel, GsVec new_vel, Agent &agent, int &check, GsVec total, int agt_num)
{
	GsVec ori_vec;
	GsVec new_vec;
	GsVec now_vec;

	float length;
	float distance;

	GsVec2 zero;
	GsVec2 one;
	GsVec2 two;
	GsVec2 three;

	float cl_pos;
	float cl_neg;

	float rad1;
	float rad2;
	float rad3;

	float angle_vec;

	if (agent.s == 0)
	{
		length = new_vel.len();
		distance = original_vel.len();
		ori_vec = original_vel;
		new_vec = new_vel;

		ori_vec.normalize();
		new_vec.normalize();

		zero = GsVec2(1, 0);
		one = GsVec2(ori_vec.x, ori_vec.y);
		two = GsVec2(new_vec.x, new_vec.y);

		rad1 = oriangle(zero, one);
		rad2 = oriangle(one, two);

		angle_vec = rad1 + rad2;

		cl_pos = angle_vec + float(GS_PI / 2.0);
		cl_neg = angle_vec - float(GS_PI / 2.0);

		if (-3.142 <= rad2 && rad2 <= -3.130)
		{
			rad2 = (-1) * float(-GS_PI);
		}

		if (3.130 <= rad2 && rad2 <= 3.142)
		{
			rad2 = 3.130f;
		}

		if ((-GS_PI / 900.0) <= rad2 && rad2 <= (GS_PI / 900.0))
		{
			agent.s = 0;
			agent.v = agent.v;
			agent.rt = agent.v;
		}
		else if ((GS_PI / 900.0) <= rad2 && rad2 <= (GS_PI / 2.0))
		{
			agent.s = 2;
			agent.nv = new_vel;
			agent.ov = original_vel;
			agent.v = length * GsVec(cos(rad1 + (GS_PI / 900.0)), sin(rad1 + (GS_PI / 900.0)), 0.0);
			agent.rt = agent.v;
		}
		else if ((-GS_PI / 2.0) <= rad2 && rad2 <= (-GS_PI / 900.0))
		{
			agent.s = 2;
			agent.nv = new_vel;
			agent.ov = original_vel;
			agent.v = length * GsVec(cos(rad1 - (GS_PI / 900.0)), sin(rad1 - (GS_PI / 900.0)), 0.0);
			agent.rt = agent.v;
		}
		else if (rad2 < -GS_PI || (GS_PI / 2.0) <= rad2 && rad2 <= GS_PI)
		{
			agent.s = 1;
			agent.nv = new_vel;
			agent.ov = original_vel;
			agent.rt = GsVec(cos(rad1 + (GS_PI / 900.0)), sin(rad1 + (GS_PI / 900.0)), 0.0);
			agent.v = GsVec::null;
		}
		else if (rad2 > GS_PI || (-GS_PI <= rad2 && rad2 <= (-GS_PI / 2.0)))
		{
			agent.s = 1;
			agent.nv = new_vel;
			agent.ov = original_vel;
			agent.rt = GsVec(cos(rad1 - (GS_PI / 900.0)), sin(rad1 - (GS_PI / 900.0)), 0.0);
			agent.v = GsVec::null;
		}
	}
	else if (agent.s == 2)
	{
		ori_vec = agent.ov;
		new_vec = agent.nv;
		now_vec = agent.rt;
		length = new_vec.len();

		zero = GsVec2(1, 0);
		one = GsVec2(ori_vec.x, ori_vec.y);
		two = GsVec2(new_vec.x, new_vec.y);
		three = GsVec2(now_vec.x, now_vec.y);

		rad1 = oriangle(zero, one);
		rad2 = oriangle(one, two);
		rad3 = oriangle(one, three);

		angle_vec = rad1 + rad2;

		cl_pos = angle_vec + float(GS_PI / 2.0);
		cl_neg = angle_vec - float(GS_PI / 2.0);

		if (0 <= rad3 && (rad3 <= rad2 && 0 < rad2))
		{
			agent.s = 2;
			agent.rt = GsVec(cos(rad1 + rad3 + (GS_PI / 900.0)), sin(rad1 + rad3 + (GS_PI / 900.0)), 0.0);
			agent.v = length * GsVec(cos(rad1 + rad3 + (GS_PI / 900.0)), sin(rad1 + rad3 + (GS_PI / 900.0)), 0.0);

			if (collide_checker(agent.x + agent.v, agt_num) == 1)
			{
				agent.v = GsVec::null;
			}
			else
			{
				agent.ed1_counter++;
			}

			if (agent.ed1_counter > max_st)
			{
				agent.s = 0;
				agent.nv = GsVec::null;
				agent.ov = GsVec::null;
				agent.ed1_counter = 0;
			}
		}
		else if ((rad2 < 0 && rad2 <= rad3) && rad3 <= 0)
		{
			agent.s = 2;
			agent.rt = GsVec(cos(rad1 + rad3 - (GS_PI / 900.0)), sin(rad1 + rad3 - (GS_PI / 900.0)), 0.0);
			agent.v = length * GsVec(cos(rad1 + rad3 - (GS_PI / 900.0)), sin(rad1 + rad3 - (GS_PI / 900.0)), 0.0);

			if (collide_checker(agent.x + agent.v, agt_num) == 1)
			{
				agent.v = GsVec::null;
			}
			else
			{
				agent.ed1_counter++;
			}

			if (agent.ed1_counter > max_st)
			{
				agent.s = 0;
				agent.nv = GsVec::null;
				agent.ov = GsVec::null;
				agent.ed1_counter = 0;
			}
		}
		else if (rad2 <= rad3 && 0 < rad2)
		{
			agent.s = 0;
			agent.nv = GsVec::null;
			agent.ov = GsVec::null;
			agent.ed1_counter = 0;
			agent.v = length * GsVec(cos(angle_vec), sin(angle_vec), 0.0);
			agent.rt = GsVec(cos(angle_vec), sin(angle_vec), 0.0);

		}
		else if (rad2 < 0 && rad2 >= rad3)
		{
			agent.s = 0;
			agent.nv = GsVec::null;
			agent.ov = GsVec::null;
			agent.ed1_counter = 0;
			agent.v = length * GsVec(cos(angle_vec), sin(angle_vec), 0.0);
			agent.rt = GsVec(cos(angle_vec), sin(angle_vec), 0.0);
		}
	}
	else if (agent.s == 1)
	{
		ori_vec = agent.ov;
		new_vec = agent.nv;
		now_vec = agent.rt;
		length = new_vec.len();
		ori_vec.normalize();
		new_vec.normalize();
		zero = GsVec2(1, 0);
		one = GsVec2(ori_vec.x, ori_vec.y);
		two = GsVec2(new_vec.x, new_vec.y);
		three = GsVec2(now_vec.x, now_vec.y);
		rad1 = oriangle(zero, one);
		rad2 = oriangle(one, two);
		rad3 = oriangle(one, three);
		angle_vec = rad1 + rad2;
		cl_pos = angle_vec + float(GS_PI / 2.0);
		cl_neg = angle_vec - float(GS_PI / 2.0);

		if (-3.142 <= rad2 && rad2 <= -3.130)
		{
			rad2 = (-1) * float(-GS_PI);
		}

		if (3.130 <= rad2 && rad2 <= 3.142)
		{
			rad2 = 3.130f;
		}

		if (0 <= rad3 && (rad3 <= rad2 && 0 < rad2))
		{
			agent.s = 1;
			agent.rt = GsVec(cos(rad1 + rad3 + (GS_PI / 900.0)), sin(rad1 + rad3 + (GS_PI / 900.0)), 0.0);
			agent.v = length * GsVec(cos(rad1 + rad3 + (GS_PI / 900.0)), sin(rad1 + rad3 + (GS_PI / 900.0)), 0.0);

			if (collide_checker(agent.x + agent.v, agt_num) == 1)
			{
				agent.v = GsVec::null;
			}
			else
			{
				agent.ed2_counter++;
			}

			if (agent.ed2_counter > max_ed)
			{
				agent.s = 0;
				agent.nv = GsVec::null;
				agent.ov = GsVec::null;
				agent.ed2_counter = 0;
			}
		}
		else if ((rad2 < 0 && rad2 <= rad3) && rad3 <= 0)
		{
			agent.s = 1;
			agent.rt = GsVec(cos(rad1 + rad3 - (GS_PI / 900.0)), sin(rad1 + rad3 - (GS_PI / 900.0)), 0.0);
			agent.v = length * GsVec(cos(rad1 + rad3 - (GS_PI / 900.0)), sin(rad1 + rad3 - (GS_PI / 900.0)), 0.0);

			if (collide_checker(agent.x + agent.v, agt_num) == 1)
			{
				agent.v = GsVec::null;
			}
			else
			{
				agent.ed2_counter++;
			}

			if (agent.ed2_counter > max_ed)
			{
				agent.s = 0;
				agent.nv = GsVec::null;
				agent.ov = GsVec::null;
				agent.ed2_counter = 0;
			}
		}
		else if (rad2 <= rad3 && 0 < rad2)
		{
			agent.s = 0;
			agent.nv = GsVec::null;
			agent.ov = GsVec::null;
			agent.ed2_counter = 0;
			agent.v = length * GsVec(cos(angle_vec), sin(angle_vec), 0.0);
			agent.rt = GsVec(cos(angle_vec), sin(angle_vec), 0.0);
		}
		else if (rad2 < 0 && rad2 >= rad3) {
			agent.s = 0;
			agent.nv = GsVec::null;
			agent.ov = GsVec::null;
			agent.ed2_counter = 0;
			agent.v = length * GsVec(cos(angle_vec), sin(angle_vec), 0.0);
			agent.rt = GsVec(cos(angle_vec), sin(angle_vec), 0.0);
		}
	}
}

// TRUNCATE FAUNCTION
GsVec ParticleSystem::truncate(GsVec vector, float max)
{
	float length = vector.len();
	vector.normalize();

	if (length > max) {
		return vector * max;
	}
	else {
		return vector * length;
	}
}

int ParticleSystem::collide_checker(GsVec position, int agt_num) {

	int i;
	float dist_length;
	int check = -1;

	for (i = 0; i < _agents.size(); i++)
	{
		if (i != agt_num) {
			dist_length = (position - _agents[i].x).len();
			if (dist_length < 51.0f)
			{
				check = 1;
			}
		}
	}
	return check;
}

int ParticleSystem::intersect_agents(GsVec ahead, GsVec ahead2, Agent &other, Agent &agent)
{
	GsVec dist;
	GsVec dist2;
	GsVec dist3;

	dist = ahead - other.x;
	dist2 = ahead2 - other.x;
	dist3 = agent.x - other.x;

	int check = -1;

	if (dist.len() < 25.0f || dist2.len() < 25.0f || dist3.len() < 50.0f)
	{
		check = 1;
	}

	return check;
}

int ParticleSystem::find_agents(Agent &agent, GsVec ahead, GsVec ahead2, int agt_num)
{
	Agent other;
	other.init(1.0f, GsVec::null, GsVec::null, GsVec::null);

	int i;
	int num = -1;
	int collide = -1;

	for (i = 0; i < _agents.size(); i++) {
		if (i != agt_num) {
			collide = intersect_agents(ahead, ahead2, _agents[i], agent);
			if (collide != -1 && (other.x == GsVec::null || dist(agent.x, _agents[i].x) < dist(agent.x, other.x)))
			{
				other = _agents[i];
				num = i;
			}
		}
	}

	return num;
}


// SEEKING
void ParticleSystem::seek(Agent &agent, GsVec &target, GsVec &sep, GsVec &align, GsVec &cohes, int agt_num)
{
	GsVec _toward;						// vector toward the target
	GsVec _steering;					// vector change the velocity direction
	GsVec _total;						// total vector that affects the agent

	float max_force = 0.0f;
	float max_avoid_force = max_velocity;
	float distance;
	float slowing_area = 50.0f;
	float avoid_area = 55.0f;

	GsVec ahead;
	GsVec ahead2;

	GsVec original_vel;
	GsVec norm_vel;

	float seen = 25.0f; // look_area
	GsVec avoid;
	float dynamic_length;

	GsVec avoid_dist;

	float sep_weight = 0.01f;

	float align_weight = 0.05f;

	float cohes_weight = 0.05f;

	int check = 0;
	int num_agt = -1;

	if (agent.v.len() < 0.000005)
	{
		original_vel = target - agent.x;
	}
	else
	{
		original_vel = agent.v;
	}

	if (agent.s == 0)
	{
		norm_vel = original_vel;
		norm_vel.normalize();

		_toward = target - agent.x;
		distance = _toward.len();

		_toward.normalize();

		// COLLISION AVOIDANCE
		if (agent.v.len() > 0.000005)
		{
			dynamic_length = agent.v.len() / max_velocity;
		}
		else
		{
			dynamic_length = 0.0f;
		}

		ahead = agent.x + norm_vel * 50.0f + norm_vel * (seen * dynamic_length);
		ahead2 = agent.x + norm_vel * 50.0f + norm_vel * (seen * 0.5f * dynamic_length);

		num_agt = find_agents(agent, ahead, ahead2, agt_num);
		agent.hit = num_agt;

		if (num_agt != -1)
		{
			avoid = ahead - _agents[num_agt].x;
			avoid.normalize();
			avoid = avoid * (50.0f / (agent.x - _agents[num_agt].x).len())*max_avoid_force;
		}
		else
		{
			avoid = GsVec::null;
		}

		// DESIRED VELOCITY
		if (distance < slowing_area)
		{
			// Inside the slowing area
			_toward = _toward * max_velocity * (distance / slowing_area);
		}
		else
		{
			// Outside the slowing area.
			_toward = _toward * max_velocity;
		}

		// FORCE FOR DESIRED VELOCITY
		_steering = _toward - agent.v;

		// TOTAL STEERING FORCE
		_steering = _steering + avoid;

		// NEW VELOCITY & POSITION
		_total = _steering + sep_weight * sep + align_weight * align + cohes_weight * cohes;

		agent.v += _total;
		agent.v = truncate(agent.v, max_velocity);
	}

	range_limit(original_vel, agent.v, agent, check, _total, agt_num);

	agent.x = agent.x + agent.v;

	if ((target - agent.x).len() < 0.5f) {
		gsout << "Agent[" << agt_num << "]  GOAL!" << gsnl;
		agent.goal = 1;
	}
}


void ParticleSystem::steering(float deltat)
{
	int i, j;
	int count;

	float sep_area = 51.0f;
	GsArray<GsVec> total_sep;
	total_sep.size(_agents.size());

	float align_area = 100.0f;
	GsArray<GsVec> total_align;
	total_align.size(_agents.size());

	float cohes_area = 100.0f;
	GsArray<GsVec> total_cohes;
	total_cohes.size(_agents.size());

	// SEPARATION 
	for (i = 0; i < _agents.size(); i++)
	{
		total_sep[i] = GsVec::null;
		count = 0;

		for (j = 0; j < _agents.size(); j++)
		{
			if (i != j || _agents[j].goal != 1)
			{
				if (dist(_agents[i].x + _agents[i].v, _agents[j].x + _agents[j].v) < sep_area)
				{
					total_sep[i] += (_agents[i].x + _agents[i].v) - (_agents[j].x + _agents[j].v);
					count++;
				}

			}
		}
	}

	// ALIGNMENT 
	for (i = 0; i < _agents.size(); i++)
	{
		total_align[i] = GsVec::null;
		count = 0;

		for (j = 0; j < _agents.size(); j++)
		{
			if (i != j)
			{
				if (dist(_agents[i].x, _agents[j].x) < sep_area)
				{
					total_align[i] += _agents[j].v;
					count++;
				}

			}
		}
		if (count != 0) {
			total_align[i] = total_align[i] / float(count);
			total_align[i].normalize();
			total_align[i] = total_align[i] * max_velocity;
		}

	}

	// COHESION
	for (i = 0; i < _agents.size(); i++)
	{
		total_cohes[i] = GsVec::null;
		count = 0;

		for (j = 0; j < _agents.size(); j++)
		{
			if (i != j)
			{
				if (dist(_agents[i].x + _agents[i].v, _agents[j].x + _agents[j].v) < sep_area)
				{
					total_cohes[i] += (_agents[j].x + _agents[j].v) - (_agents[i].x + _agents[i].v);
					count++;
				}

			}
		}
		if (count != 0) {
			total_cohes[i] = total_cohes[i] / float(count);
			total_cohes[i].normalize();
			total_cohes[i] = total_cohes[i] * max_velocity;
		}

	}

	// SEEKING
	for (i = 0; i < _agents.size(); i++)
	{
		if (_agents[i].goal == 0) {
			//gsout << "Agent[" << i << "]:" << gsnl ;
			seek(_agents[i], _targets[i], total_sep[i], total_align[i], total_cohes[i], i);
			//gsout << gsnl;
		}
	}
}

//======================================== EOF ===============================================
