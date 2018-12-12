#ifndef CVAM
#define CVAM
#include "operation.hpp"
#include "header.hpp"
typedef map<coeff_t, option_vec_t> coeff_map_vec_t;
void printT(coeff_map_vec_t T, option_vec_t y);
coeff_map_vec_t CONV2CAVM(coeff_map_t O, int mlc, option_vec_t y);
#endif // !CVAM