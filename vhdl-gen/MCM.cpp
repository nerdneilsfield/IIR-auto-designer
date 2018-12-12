#include "MCM.hpp"

cost_t SUCCS_size = 0;
cost_t COST = 0;
coeff_set_t COST0, COST1, COST2;
dist_map_t DISTS, ESTDISTS;
map<coeff_t, int> DEPTH_LIMITS;
coeff_set_t READY, WORKLIST, SUCCS, TARGETS;
coeff_set_t TARGETS_SAVE;

inline int MAX_TARGET_DEPTH(coeff_t t) {
	return DEPTH_LIMITS.find(t) == DEPTH_LIMITS.end() ? -1 : DEPTH_LIMITS[t];
}

void set_union(coeff_set_t A, coeff_set_t B, coeff_set_t &F)
{
	for (coeff_set_t::iterator a = A.begin(); a != A.end(); a++)
	{
		F.insert(*a);
	}
	for (coeff_set_t::iterator b = B.begin(); b != B.end(); b++)
	{
		F.insert(*b);
	}
}
void set_difference(coeff_set_t A, coeff_set_t B, coeff_set_t &F)
{
	for (coeff_set_t::iterator a = A.begin(); a != A.end(); a++)
	{
		if (B.count(*a) == 0)
		{
			F.insert(*a);
		}
	}
}
void set_intersection(coeff_set_t A, coeff_set_t B, coeff_set_t &F)
{
	for (coeff_set_t::iterator a = A.begin(); a != A.end(); a++)
	{
		if (B.count(*a) == 1)
		{
			F.insert(*a);
		}
	}
}

cost_t getCSDCost(coeff_t c)
{
	c = (c << 1);
	int res = 0, mode = 0, newmode;
	while (c) {
		if (mode == 0)
		{
			newmode = 0;
			if ((c & 3) == 1)
				++res;
			if ((c & 3) == 3)
			{
				++res;
				newmode = 1; 
			}
		}
		else
		{
			newmode = 1;
			if ((c & 3) == 0)
			{ 
				++res;
				newmode = 0;
			}
			if ((c & 3) == 2)
				++res;
		}
		mode = newmode;
		c = (c >> 1);
	}
	if (mode == 1)
		++res;
	return res - 1;
}

cost_t Est(coeff_t z)
{
	return getCSDCost(z);
}
cost_t Est(coeff_set_t Z)
{
	set<cost_t> temp_cost_set;
	for (coeff_set_t::iterator z = Z.begin(); z != Z.end(); z++)
	{
		temp_cost_set.insert(Est(*z));
	}
	return *temp_cost_set.begin();
}

/* 计算Ap基本值中L1或L2为0的情况，注意如果L1或L2不为0，则可与READY约掉*/
void _compute_Ap(coeff_set_t &result, coeff_t num1, coeff_t num2)
{
	int max_shift = MAX_SHIFT - clog2(num2);
	if (max_shift < 0) max_shift = 0;
	for (int shift = MIN_SHIFT; shift <= max_shift; ++shift) {
		coeff_t sum, diff;
		diff = cfabs(num1 - (num2 << shift));
		if (diff != 0) {
			sum = num1 + (num2 << shift);			
			if (diff < MAX_NUM)
			{
				result.insert(fundamental(diff));
			}
			if (sum < MAX_NUM)
			{
				result.insert(fundamental(sum));
			}
		}
	}
}
void _compute_Ap(coeff_t num1, coeff_t num2, coeff_map_t &Dictionary)
{
	int max_shift = MAX_SHIFT - clog2(num2);
	if (max_shift < 0) 
		max_shift = 0;

	int level = Dictionary[num1].level > Dictionary[num2].level ? Dictionary[num1].level + 1 : Dictionary[num2].level + 1;

	for (int shift = MIN_SHIFT; shift <= max_shift; ++shift) {
		coeff_t sum, diff;
		diff = cfabs(num1 - (num2 << shift));
		if (diff != 0) {
			sum = num1 + (num2 << shift);
			if (diff != 1 && diff < MAX_NUM && READY.count(diff) == 1 && Dictionary[fundamental(diff)].level > level)
			{
				Dictionary[fundamental(diff)] = *option::subshl(fundamental(diff), num1, num2, shift, level);
			}
			if (sum != 1 && sum < MAX_NUM && READY.count(sum) == 1 && Dictionary[fundamental(diff)].level > level)
			{
				Dictionary[fundamental(sum)] = *option::addshl(fundamental(sum), num1, num2, shift, level);
			}
		}
	}
}

void compute_Ap(coeff_t num1, coeff_t num2, coeff_set_t &result)
{
	_compute_Ap(result, num1, num2);
	_compute_Ap(result, num2, num1);
}
void compute_Ap(coeff_t num1, coeff_t num2, coeff_map_t &Dictionary)
{
	_compute_Ap(num1, num2, Dictionary);
	_compute_Ap(num2, num1, Dictionary);
}
void compute_Ap(coeff_t num1, coeff_set_t &src2, coeff_set_t &dest) 
{
	for (coeff_set_t::iterator jt = src2.begin(); jt != src2.end(); ++jt)
	{
		compute_Ap(num1, *jt, dest);
	}
}

void compute_Ap(coeff_set_t &src1, coeff_set_t &src2, coeff_set_t &dest) 
{
	for (coeff_set_t::iterator it = src1.begin(); it != src1.end(); ++it)
	{
		for (coeff_set_t::iterator jt = src2.begin(); jt != src2.end(); ++jt)
		{
			compute_Ap(*it, *jt, dest);
		}
	}		
}
void compute_Ap(coeff_set_t &src1, coeff_set_t &src2, coeff_map_t &Dictionary)
{
	for (coeff_set_t::iterator it = src1.begin(); it != src1.end(); ++it)
	{
		for (coeff_set_t::iterator jt = src2.begin(); jt != src2.end(); ++jt)
		{
			compute_Ap(*it, *jt, Dictionary);
		}
	}
}

bool distance2_Test(coeff_set_t R, coeff_t t)
{
	bool isDist2 = false;
Case_1:
	for (coeff_set_t::iterator c1 = COST1.begin(); c1 != COST1.end(); c1++)
	{
		coeff_t temp_s = t / (*c1);
		if (temp_s*(*c1) == t && SUCCS.count(temp_s) == 1)
		{
			DISTS[coeff_pair_t(1, t)] = 2;
			DISTS[coeff_pair_t(temp_s, t)] = 1;
			isDist2 = true;
		}
	}
Case_2:
	coeff_set_t temp_AstR;
	compute_Ap(t, R, temp_AstR);
	coeff_set_t temp_AstRintersectS;
	set_intersection<coeff_set_t::iterator>(temp_AstR.begin(), temp_AstR.end(), SUCCS.begin(), SUCCS.end(), inserter(temp_AstRintersectS, temp_AstRintersectS.end()));
	for (coeff_set_t::iterator sd2 = temp_AstRintersectS.begin(); sd2 != temp_AstRintersectS.end(); sd2++)
	{
		DISTS[coeff_pair_t(1, t)] = 2;
		DISTS[coeff_pair_t(*sd2, t)] = 1;
		isDist2 = true;
	}
	return isDist2;
}

bool distance3_Test(coeff_set_t R, coeff_t t)
{
	bool isDist3 = false;
Case_1_and_2:
	for (coeff_set_t::iterator c2 = COST2.begin(); c2 != COST2.end(); c2++)
	{
		coeff_t temp_s = t / (*c2);
		if (temp_s*(*c2) == t && SUCCS.count(temp_s) == 1)
		{
			DISTS[coeff_pair_t(1, t)] = 3;
			DISTS[coeff_pair_t(temp_s, t)] = 2;
			isDist3 = true;
		}
	}
Case_3:
	for (coeff_set_t::iterator c1 = COST1.begin(); c1 != COST1.end(); c1++)
	{
		coeff_t temp_tdc = t / (*c1);
		if (temp_tdc*(*c1) != t)
		{
			continue;
		}
		coeff_set_t temp_AstdcR;
		compute_Ap(temp_tdc, R, temp_AstdcR);
		coeff_set_t temp_AstdcRintersectS;
		set_intersection<coeff_set_t::iterator>(temp_AstdcR.begin(), temp_AstdcR.end(), SUCCS.begin(), SUCCS.end(), inserter(temp_AstdcRintersectS, temp_AstdcRintersectS.end()));
		for (coeff_set_t::iterator sd3 = temp_AstdcRintersectS.begin(); sd3 != temp_AstdcRintersectS.end(); sd3++)
		{
			DISTS[coeff_pair_t(1, t)] = 3;
			DISTS[coeff_pair_t(*sd3, t)] = 2;
			isDist3 = true;
		}
	}
Case_4:
	for (coeff_set_t::iterator c1 = COST1.begin(); c1 != COST1.end(); c1++)
	{
		coeff_set_t temp_AstR;
		compute_Ap(t, R, temp_AstR);
		for (coeff_set_t::iterator AsRt = temp_AstR.begin(); AsRt != temp_AstR.end(); AsRt++)
		{
			coeff_t temp_s = (*AsRt) / (*c1);
			if (temp_s*(*c1) == t && SUCCS.count(temp_s) == 1)
			{
				DISTS[coeff_pair_t(1, t)] = 3;
				DISTS[coeff_pair_t(temp_s, t)] = 2;
				isDist3 = true;
			}
		}
	}
Case_5:
	coeff_set_t temp_AstS;
	compute_Ap(t, SUCCS, temp_AstS);
	coeff_set_t temp_AstSintersectS;
	set_intersection<coeff_set_t::iterator>(temp_AstS.begin(), temp_AstS.end(), SUCCS.begin(), SUCCS.end(), inserter(temp_AstSintersectS, temp_AstSintersectS.end()));
	for (coeff_set_t::iterator sd3 = temp_AstSintersectS.begin(); sd3 != temp_AstSintersectS.end(); sd3++)
	{
		DISTS[coeff_pair_t(1, t)] = 3;
		DISTS[coeff_pair_t(*sd3, t)] = 2;
		isDist3 = true;
	}
	return isDist3;
}

inline long lookup_dist(coeff_t succ, coeff_t t) 
{
	int d1 = -1, d2 = -1;
	dist_map_t::const_iterator d;

	d = DISTS.find(coeff_pair_t(succ, t));
	if (d != DISTS.end()) 
		d1 = (*d).second;

	d = ESTDISTS.find(coeff_pair_t(succ, t));
	if (d != ESTDISTS.end()) 
		d2 = (*d).second;

	if (d1 == -1) return d2;
	else if (d2 == -1) return d1;
	else return min(d1, d2);
}
inline long lookup_dist(coeff_t t) 
{
	dist_map_t::const_iterator d = DISTS.find(coeff_pair_t(1, t));
	if (d != DISTS.end()) return (*d).second;
	else return -1;
}
void exact_target_dists()
{
	for (coeff_set_t::iterator t = TARGETS.begin(); t != TARGETS.end(); t++)
	{
		long dist = lookup_dist(1, *t);

		if (dist == 1)
		{
			continue;
		}
		else if (distance2_Test(READY, *t))
		{
			continue;
		}
		else if (distance3_Test(READY, *t))
		{
			continue;
		}
		else
		{
			DISTS[coeff_pair_t(1, *t)] = 4;
		}			
	}
}
int estimate_target_dist(coeff_t s, coeff_t t)
{
	cost_t min_cost = INT_MAX;
	coeff_t aux = 0;

Case_1:
	coeff_set_t temp_Asst;
	compute_Ap(s, t, temp_Asst);
	min_cost = 1 + Est(temp_Asst);
	if (min_cost == 3)
	{
		ESTDISTS[coeff_pair_t(s, t)] = 3;
		return 3;
	}
Case_2:
	for (coeff_set_t::iterator c1 = COST1.begin(); c1 != COST1.end(); c1++)
	{
		if (t % (*c1) != 0)
		{
			continue;
		}
		coeff_set_t temp_AsstC1;
		compute_Ap(s, t / (*c1), temp_AsstC1);
		cost_t temp_cost = 2 + Est(temp_AsstC1);
		if (temp_cost < min_cost)
		{
			min_cost = temp_cost;
			aux = t / (*c1);
			if (min_cost == 3)
			{
				ESTDISTS[coeff_pair_t(s, t)] = 3;
				return 3;
			}
		}
	}
Case_3:
	for (coeff_set_t::iterator c1 = COST1.begin(); c1 != COST1.end(); c1++)
	{
		coeff_set_t temp_AsC1st;
		compute_Ap(s * (*c1), t, temp_AsC1st);
		cost_t temp_cost = 2 + Est(temp_AsC1st);
		if (temp_cost < min_cost)
		{
			min_cost = temp_cost;

			if (min_cost == 3)
			{
				ESTDISTS[coeff_pair_t(s, t)] = 3;
				return 3;
			}
		}
	}
/*
Case_4:
	coeff_set_t temp_AsRst;
	compute_Ap(READY, temp_Asst, temp_AsRst);
	cost_t E4 = 2 + Est(temp_AsRst);
	if (E4 < min_cost)
	{
		min_cost = E4;
		if (min_cost == 3)
		{
			ESTDISTS[coeff_pair_t(s, t)] = dist_t(3, 0, -1);
			return 3;
		}
	}
*/
	if (min_cost < INT_MAX && aux > 1)
	{
		ESTDISTS[coeff_pair_t(s, aux)] = min_cost - 1;
	}
	return min_cost;
}

void update_target_dists(coeff_t succ) {
	for (coeff_set_t::iterator t = TARGETS.begin(); t != TARGETS.end(); t++)
	{
		long dist = lookup_dist(succ, *t);
		if (dist > 0) {
			long orig_dist = lookup_dist(1, *t);
			if (orig_dist > dist)
				DISTS[coeff_pair_t(succ, *t)] = dist;
		}
	}
}

coeff_pair_t cumulative_benefit(coeff_t succ)
{
	coeff_t total_benefit = 0;
	coeff_t max_benefit = 0;
	coeff_t weighted_B;
	coeff_t closest = 0;

	if (TARGETS.count(succ) == 1)  
		return coeff_pair_t(INT_MAX, 0);

	for (coeff_set_t::iterator t = TARGETS.begin(); t != TARGETS.end(); t++)
	{
		coeff_t R_t_dist = lookup_dist(*t);
		coeff_t Rs_t_dist = lookup_dist(succ, *t);

		if (Rs_t_dist == -1) {
			if (R_t_dist <= 3) 
				Rs_t_dist = R_t_dist;
			else 
				Rs_t_dist = estimate_target_dist(succ, *t);
		}

		if (Rs_t_dist < R_t_dist)
			weighted_B = (R_t_dist - Rs_t_dist) * pow(13.0, 11.0 - Rs_t_dist);
		else 
			weighted_B = 0;

		total_benefit += weighted_B;
		if (weighted_B > max_benefit)
		{ 
			max_benefit = weighted_B;
			closest = *t;
		}
	}
	return coeff_pair_t(total_benefit, closest);
}

coeff_t Heuristic_CumulativeBenefit()
{
	coeff_t s_argmax;
	coeff_t cumuB_max = 0;
	for (coeff_set_t::iterator s = SUCCS.begin(); s != SUCCS.end(); s++)
	{
		coeff_t temp_cumuB = cumulative_benefit(*s).first;
		if (temp_cumuB > cumuB_max)
		{
			s_argmax = *s;
			cumuB_max = temp_cumuB;
		}
	}
	update_target_dists(s_argmax);
	return s_argmax;
}

void Synthesize(coeff_t s)
{
	WORKLIST.insert(s);
	TARGETS.erase(s);
	// keep patient
	cout << "  Number of elements in T: " << TARGETS.size() << endl;
}

void SynthesizeMultiplierBlock(coeff_vec_t Given)
{
	//1: Right shift elements of TARGETS until odd
	for (coeff_vec_t::iterator it = Given.begin(); it != Given.end(); ++it)
	{
		if (*it == 0 || *it == 1)
		{
			continue;
		}
		while (*it % 2 == 0)
		{
			*it = *it >> 1;
		}
		TARGETS.insert(fundamental(*it));
	}
	TARGETS_SAVE = TARGETS;

	//2: R ← {1}
	READY.insert(1);
	//3: W ← {1}
	WORKLIST.insert(1);
	//4: S ← {1}
	SUCCS.insert(1);

	//5: while T != null_set do
	while (!TARGETS.empty())
	{
	//6: {optimal part}
	Optimal_part:
		//7: while W != null_set do
		while (!WORKLIST.empty())
		{
		//8: {update S and R}
		Update_S_and_R:
			//9: R ← R ∪ W
			coeff_set_t READY_temp;
			set_union<coeff_set_t::iterator>(READY.begin(), READY.end(), WORKLIST.begin(), WORKLIST.end(), inserter(READY_temp, READY_temp.end()));
			READY = READY_temp;
			//10: S ← (S ∪ A*(R,W)) - W
			coeff_set_t temp_AsRW;
			compute_Ap(READY, WORKLIST, temp_AsRW);
			coeff_set_t temp_SunionAs;
			set_union<coeff_set_t::iterator>(SUCCS.begin(), SUCCS.end(), temp_AsRW.begin(), temp_AsRW.end(), inserter(temp_SunionAs, temp_SunionAs.end()));
			set_difference<coeff_set_t::iterator>(temp_SunionAs.begin(), temp_SunionAs.end(), WORKLIST.begin(), WORKLIST.end(), inserter(SUCCS, SUCCS.end()));
			if (SUCCS.size() > SUCCS_size)
			{
				SUCCS_size = SUCCS.size();
			}
			//11: W ← null_set
			WORKLIST.clear();

			//12: {if S contains targets, synthesize them}
		Synthesize_targets_in_S:
			coeff_set_t temp_SunionT;
			set_intersection<coeff_set_t::iterator>(SUCCS.begin(), SUCCS.end(), TARGETS.begin(), TARGETS.end(), inserter(temp_SunionT, temp_SunionT.end()));
			//13: for t ∈ S ∩ T do
			for (coeff_set_t::iterator t = temp_SunionT.begin(); t != temp_SunionT.end(); ++t)
			{
				//14: Synthesize(T)
				Synthesize(*t);
			}
		}
	//15: {heuristic part}
	Heuristic_part:
		exact_target_dists();
		//16: if T != null_set then
		if (!TARGETS.empty())
		{
			//17: s ← H(R, S, T)
			coeff_t temp_s = Heuristic_CumulativeBenefit();
			//18: Synthesize(s)
			Synthesize(temp_s);
		}
	}
}

void computeCost()
{
	COST0.insert(1);
	compute_Ap(COST0, COST0, COST1);
	cout << ".";
	compute_Ap(COST0, COST1, COST2);
	cout << "..";
	for (coeff_set_t::iterator c11 = COST1.begin(); c11 != COST1.end(); c11++)
	{
		for (coeff_set_t::iterator c12 = COST1.begin(); c12 != COST1.end(); c12++)
		{
			if (clog2(*c11) + clog2(*c12) > clog2(MAX_NUM) - 1)
			{
				continue;
			}
			else
			{
				coeff_t temp_prod = (*c11) * (*c12);
				if (temp_prod < MAX_NUM && temp_prod > 0)
				{
					COST2.insert(temp_prod);
				}
			}
		}
	}
	cout << "..";
	COST1.erase(1);
	COST2.erase(1);
	coeff_set_t COST2_temp;
	set_difference<coeff_set_t::iterator>(COST2.begin(), COST2.end(), COST1.begin(), COST1.end(), inserter(COST2_temp, COST2_temp.end()));
	COST2 = COST2_temp;
	cout << ".";
}
void output(option temp_option, coeff_map_t Dictionary)
{
	if (temp_option.res != 1 && temp_option.res != 0)
	{
		temp_option.output();
		COST++;
	}
}

coeff_map_t main_MCM(coeff_vec_t coeff_vec)
{
	cout << "Please wait for the calculation of COST array to be completed..";
	computeCost();
	cout << "Done" << endl;

	cout << "Computing..." << endl;
	SynthesizeMultiplierBlock(coeff_vec);
	cout << "Done" << endl;

	cout << "*************************************" << endl;
	coeff_map_t Dictionary;
	Dictionary[1] = *option::addone();
	compute_Ap(READY, READY, Dictionary);
	coeff_set_t temp_RsubT;
	set_difference(READY, TARGETS_SAVE, temp_RsubT);
	compute_Ap(temp_RsubT, temp_RsubT, Dictionary);
	TARGETS.insert(1);
	compute_Ap(TARGETS, TARGETS, Dictionary);
	

	for (coeff_vec_t::iterator r = coeff_vec.begin(); r != coeff_vec.end(); r++)
	{
		output(Dictionary[*r], Dictionary);
	}
	cout << "*************************************" << endl;
	cout << "Add or Sub Cost: " << COST << endl;
	cout << "Added nodes: " << endl << "    ";
	for (coeff_set_t::iterator rst = temp_RsubT.begin(); rst != temp_RsubT.end(); rst++)
	{
		cout << *rst << "  ";
	}
	cout << endl;
	cout << "Biggest size of SUCCS: " << SUCCS_size << endl;
	cout << "*************************************" << endl;
	return Dictionary;
}