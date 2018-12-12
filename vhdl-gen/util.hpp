#ifndef UTIL
#define UTIL
#include "header.hpp"

inline coeff_t cfabs(coeff_t x) { return x >= 0 ? x : -x; }

inline coeff_t fundamental(coeff_t z) {
  z = cfabs(z);
  while ((z & (coeff_t)1) == 0) z = (z >> ((coeff_t)1));
  return z;
}

/* floor(log_2(c)) */
inline int flog2(coeff_t c) {
  // _ASSERT(c > 0);
  int res = -1;
  while (c != 0) {
    c = (c >> ((coeff_t)1));
    ++res;
  }
  return res;
}

/* ceil(log_2(c)) */
inline int clog2(coeff_t c) {
  int floor = flog2(c);
  return floor + ((c == (1 << floor)) ? 0 : 1);
}

#endif  // !UTIL
