#ifndef HEADER
#define HEADER
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <vector>
#include <cmath>
#include <list>
#include <set>
#include <map>

constexpr auto MIN_SHIFT = 0;
constexpr auto MAX_SHIFT = 16;
constexpr auto MAX_NUM = 1 << MAX_SHIFT;

using namespace std;

typedef int							cost_t;
typedef long long					coeff_t;
typedef set<coeff_t>				coeff_set_t;
typedef vector<coeff_t>				coeff_vec_t;
typedef pair<coeff_t, coeff_t>		coeff_pair_t;
typedef map<coeff_pair_t, long>		dist_map_t;

#endif // !HEADER