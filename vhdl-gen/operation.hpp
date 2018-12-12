#ifndef OPERATION
#define OPERATION
#include "header.hpp"
struct option {
  /*
          mulshl: res = r1 *  r2 << sh
          addshl: res = r1 + (r2 << sh)
          subshl: res = r1 - (r2 << sh)
  */
  typedef int reg_t;
  typedef enum { MULSHL, ADDSHL, SUBSHL } type_t;

  type_t type;
  reg_t r1, r2, res;
  int sh;
  int level;

  option() : type(MULSHL), r1(0), r2(0), res(0), sh(0), level(99) {}

  static option* mulshl(reg_t res, reg_t r1, reg_t r2, int sh, int level = 99) {
    return new option(MULSHL, r1, r2, res, sh, level);
  }
  static option* addshl(reg_t res, reg_t r1, reg_t r2, int sh, int level = 99) {
    return new option(ADDSHL, r1, r2, res, sh, level);
  }
  static option* subshl(reg_t res, reg_t r1, reg_t r2, int sh, int level = 99) {
    return new option(SUBSHL, r1, r2, res, sh, level);
  }
  static option* addone() { return new option(ADDSHL, 1, 1, 1, 0, 0); }

  void output() {
    cout << res << " = ";
    switch (type) {
      case MULSHL:
        cout << r1 << " * (" << r2 << " << " << sh << ");";
        break;
      case ADDSHL:
        cout << r1 << " + (" << r2 << " << " << sh << ");";
        break;
      case SUBSHL:
        cout << r1 << " - (" << r2 << " << " << sh << ");";
        break;
        // default: _ASSERT(0);
    }
    cout << endl;
  }
  reg_t update(int real_r2) {
    switch (type) {
      case MULSHL:
        res = r1 * real_r2 << sh;
        break;
      case ADDSHL:
        res = r1 + (real_r2 << sh);
        break;
      case SUBSHL:
        res = r1 - (real_r2 << sh);
        break;
        // default: _ASSERT(0);
    }
    return res;
  }

 private:
  option(type_t tt, reg_t rr1, reg_t rr2, reg_t rres, int ssh, int llevel)
      : type(tt), r1(rr1), r2(rr2), res(rres), sh(ssh), level(llevel) {}
};

typedef map<coeff_t, option> coeff_map_t;
typedef vector<option> option_vec_t;
#endif  // !OPERATION