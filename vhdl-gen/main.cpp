#include "CAVM.hpp"
#include "MCM.hpp"

int mlc = 0;
int initial_length = 8;
map<int, int> length_map;
map<int, int> maximum_map;
string entityName = "TOP";

struct signal {
  string name;
  int length;
  vector<option> expression;
};

typedef struct signal signal;

map<int, int> max_value_map;

void initialMaxValue(int y_size) {
  for (int j = 1; j <= y_size; j++) {
    max_value_map[j] = 0;
  }
}

void genHeadAndStruct(int y_size) {
  cout << "library IEEE;" << endl;
  cout << "use IEEE.STD_LOGIC_1164.ALL;" << endl;
  cout << "use IEEE.NUMERIC_STD.all;" << endl;
  cout << endl;
  cout << "entity " << entityName << " is" << endl;
  cout << "    Port ( CLK : in STD_LOGIC;" << endl;
  cout << "           x : in SIGNED (" << initial_length - 1 << " downto 0);"
       << endl;
  cout << "           y : out SIGNED (" << length_map[y_size + 1] - 1
       << " downto 0));" << endl;
  cout << "end TOP;";
  cout << endl;
  cout << "architecture Behavioral of " << entityName << " is" << endl;
  cout << "    component REG is" << endl;
  cout << "        Generic ( n : INTEGER := " << initial_length - 1 << ");"
       << endl;
  cout << "        Port ( CLK : in STD_LOGIC;" << endl;
  cout << "               D : in SIGNED (n-1 downto 0);" << endl;
  cout << "               Q : out SIGNED (n-1 downto 0));" << endl;
  cout << "    end component;" << endl;
}
void genDeclareX(int y_size) {
  for (int i = 0; i < y_size; i++) {
    cout << "    signal " << (i > 9 ? "x" : "x0") << to_string(i) << ":SIGNED ("
         << initial_length - 1 << " downto 0);" << endl;
  }
  cout << endl;
}
void genREG(int y_size) {
  cout << "reg_x00: REG generic map(n=>" << initial_length
       << ") port map(CLK=>CLK,D=>x,Q=>x00);" << endl
       << endl;
  for (int i = 0; i < y_size - 1; i++) {
    string D = (i > 9 ? "x" : "x0") + to_string(i);
    string Q = (i > 8 ? "x" : "x0") + to_string(i + 1);
    cout << "reg_" << Q << ": REG generic map(n=>" << initial_length
         << ") port map(CLK=>CLK,D=>" << D << ",Q=>" << Q << ");" << endl;
  }
}

int inputX(int index) {
  return index > 0 ? max_value_map[index] : -max_value_map[-index];
}
int calculateOptionValue(coeff_map_vec_t T, option_vec_t y, int y_size) {
  int result = 0;
  int r2;
  for (int i = 0; i < y.size(); i++) {
    if (abs(y[i].r2) > y_size) {
      if (y[i].r2 > 0) {
        r2 = calculateOptionValue(T, T[y[i].r2], y_size);
      } else {
        r2 = -calculateOptionValue(T, T[-y[i].r2], y_size);
      }
    } else {
      r2 = inputX(y[i].r2);
    }
    result += y[i].update(r2);
  }
  return result;
}
int getMaxValue(coeff_map_vec_t T, option_vec_t y_temp, int y_size,
                bool isNeg) {
  initialMaxValue(y_size);
  for (int j = 1; j <= y_size; j++) {
    int y_before = calculateOptionValue(T, y_temp, y_size);
    max_value_map[j] = 1;
    int y_after = calculateOptionValue(T, y_temp, y_size);

    if (y_before > y_after) {
      max_value_map[j] = isNeg ? 127 : -128;
    } else if (y_before < y_after) {
      max_value_map[j] = isNeg ? -128 : 127;
    } else {
      max_value_map[j] = 0;
    }
  }
  return abs(calculateOptionValue(T, y_temp, y_size));
}
int getMaxValue(coeff_map_vec_t T, option_vec_t y_temp, int y_size) {
  return max(getMaxValue(T, y_temp, y_size, true),
             getMaxValue(T, y_temp, y_size, false));
}

string fixName_sh(string rPname, int sh, int sum_length,
                  vector<string> &fix_vec, vector<string> &declear_fix_vec) {
  string rPname_fix = rPname;
  rPname_fix += "_sh" + to_string(sh);

  string declear = "signal " + rPname_fix + " : SIGNED(" +
                   to_string(sum_length - 1) + " downto 0);";
  string fix =
      rPname_fix + " <= shift_left(" + rPname + ", " + to_string(sh) + ");";

  declear_fix_vec.push_back(declear);
  fix_vec.push_back(fix);

  return rPname_fix;
}
string fixName_len(string rPname, int sum_length, vector<string> &fix_vec,
                   vector<string> &declear_fix_vec) {
  string rPname_fix = rPname;
  rPname_fix += "_len" + to_string(sum_length);
  string declear = "signal " + rPname_fix + " : SIGNED(" +
                   to_string(sum_length - 1) + " downto 0);";
  string fix =
      rPname_fix + " <= resize(" + rPname + ", " + rPname_fix + "'length);";

  declear_fix_vec.push_back(declear);
  fix_vec.push_back(fix);

  return rPname_fix;
}
string fixName_neg(string rPname, int sig_length, vector<string> &fix_vec,
                   vector<string> &declear_fix_vec) {
  string rPname_fix = rPname + "_N";
  string declear = "signal " + rPname_fix + " : SIGNED(" +
                   to_string(sig_length - 1) + " downto 0);";
  string fix = rPname_fix + " <= -" + rPname + ";";

  declear_fix_vec.push_back(declear);
  fix_vec.push_back(fix);

  return rPname_fix;
}
signal dealOption(int y_size, option temp, int sum_length,
                  vector<string> &fix_vec, vector<string> &declear_fix_vec) {
  signal sig;
  sig.expression.push_back(temp);
  sig.length = sum_length;

  string name;
  int pre_length;
  bool isNeg = temp.r2 < 0;
  int sh = temp.sh;

  if (abs(temp.r2) > y_size) {
    name = (abs(temp.r2) - y_size > 9 ? "y" : "y0") +
           to_string(abs(temp.r2) - y_size);
    pre_length = length_map[abs(temp.r2)];
  } else {
    name = (abs(temp.r2) > 10 ? "x" : "x0") + to_string(abs(temp.r2) - 1);
    pre_length = initial_length;
  }

  if (sum_length > pre_length) {
    name = fixName_len(name, sum_length, fix_vec, declear_fix_vec);
  }
  if (isNeg) {
    name = fixName_neg(name, sum_length, fix_vec, declear_fix_vec);
  }
  if (sh > 0) {
    name = fixName_sh(name, sh, sum_length, fix_vec, declear_fix_vec);
  }

  sig.name = name;
  return sig;
}
signal addSignal(signal A, signal B, int i, int depth, int index,
                 int sum_length, vector<option> expression,
                 vector<string> &fix_vec, vector<string> &add_vec,
                 vector<string> &declear_fix_vec,
                 vector<string> &declear_add_vec) {
  signal sum;
  string sign =
      to_string(i + 1) + "_d" + to_string(depth) + "_i" + to_string(index);
  sum.name = (i > 8 ? "sum_y" : "sum_y0") + sign;
  sum.expression = expression;
  sum.length = sum_length;

  string A_name = A.name;
  string B_name = B.name;
  if (sum_length > A.length) {
    A_name = fixName_len(A_name, sum_length, fix_vec, declear_add_vec);
  }
  if (sum_length > B.length) {
    B_name = fixName_len(B_name, sum_length, fix_vec, declear_add_vec);
  }

  string declear = "signal " + sum.name + " : SIGNED(" +
                   to_string(sum_length - 1) + " downto 0);";
  string add =
      "ADD" + sign + ": " + sum.name + " <= " + A_name + " + " + B_name + ";";

  declear_add_vec.push_back(declear);
  add_vec.push_back(add);

  return sum;
}
void genVHDLCodePerY(coeff_map_vec_t T, int y_size, int i,
                     vector<string> &fix_add_vec,
                     vector<string> &declear_fix_vec,
                     vector<string> &declear_y_vec,
                     vector<string> &declear_add_vec) {
  string yName = (i >= 9 ? "y" : "y0") + to_string(i + 1);
  fix_add_vec.push_back("\n--**********************************************");
  fix_add_vec.push_back("--***Build Addition tree to calculat node " + yName +
                        "***");
  fix_add_vec.push_back("--**********************************************");

  vector<signal> node_vec;

  option_vec_t y_temp = T[y_size + i + 1];

  int max_sum;
  int sum_len;

  bool remain = y_temp.size() % 2 == 1;
  int y_temp_size = remain ? y_temp.size() - 1 : y_temp.size();
  for (int j = 1; j <= y_temp_size; j++) {
    option temp_A = y_temp[j - 1];
    j++;
    option temp_B = y_temp[j - 1];

    option_vec_t temp_option_vec;
    temp_option_vec.push_back(temp_A);
    temp_option_vec.push_back(temp_B);

    max_sum = getMaxValue(T, temp_option_vec, y_size);
    sum_len = ceil(log2((double)max_sum + 1)) + 1;

    signal sig_A =
        dealOption(y_size, temp_A, sum_len, fix_add_vec, declear_fix_vec);
    signal sig_B =
        dealOption(y_size, temp_B, sum_len, fix_add_vec, declear_fix_vec);

    signal sig_sum =
        addSignal(sig_A, sig_B, i, 0, j / 2, sum_len, temp_option_vec,
                  fix_add_vec, fix_add_vec, declear_fix_vec, declear_add_vec);
    fix_add_vec.push_back("\t");
    node_vec.push_back(sig_sum);
  }

  int depth = 1;
  vector<signal> node_vec_next;
  while (remain || node_vec.size() > 1) {
    if (remain && node_vec.size() % 2 == 1) {
      option temp_option = y_temp.back();
      signal temp_signal = dealOption(
          y_size, temp_option,
          (abs(temp_option.r2) > y_size ? length_map[abs(temp_option.r2)]
                                        : initial_length) +
              temp_option.sh,
          fix_add_vec, declear_fix_vec);
      node_vec.push_back(temp_signal);
      remain = false;
    }

    bool single = node_vec.size() % 2 == 1;
    int node_vec_size = single ? node_vec.size() - 1 : node_vec.size();
    for (int j = 1; j <= node_vec_size; j++) {
      signal sig_A = node_vec[j - 1];
      j++;
      signal sig_B = node_vec[j - 1];

      option_vec_t temp_option_vec;
      for (int k = 0; k < sig_A.expression.size(); k++) {
        temp_option_vec.push_back(sig_A.expression[k]);
      }
      for (int k = 0; k < sig_B.expression.size(); k++) {
        temp_option_vec.push_back(sig_B.expression[k]);
      }

      max_sum = getMaxValue(T, temp_option_vec, y_size);
      sum_len = ceil(log2((double)max_sum + 1)) + 1;

      signal sig_sum =
          addSignal(sig_A, sig_B, i, depth, j / 2, sum_len, temp_option_vec,
                    fix_add_vec, fix_add_vec, declear_fix_vec, declear_add_vec);
      fix_add_vec.push_back("\t");
      node_vec_next.push_back(sig_sum);
    }
    if (single) {
      node_vec_next.push_back(node_vec.back());
    }
    node_vec = node_vec_next;
    node_vec_next.clear();
    depth++;
  }
  length_map[i + y_size + 1] = sum_len;
  maximum_map[i + y_size + 1] = max_sum;
  string declear =
      "signal " + yName + " : SIGNED(" + to_string(sum_len - 1) + " downto 0);";
  string fix = yName + " <= " + node_vec[0].name + ";";
  declear_y_vec.push_back(declear);
  fix_add_vec.push_back(fix);

  // fix_add_vec.push_back("--**********************************************");
}

void genVHDLCode(coeff_map_vec_t T, int y_size) {
  vector<string> fix_add_vec;
  vector<string> declear_fix_vec;
  vector<string> declear_add_vec;
  vector<string> declear_y_vec;

  for (int i = 1; i < T.size(); i++) {
    genVHDLCodePerY(T, y_size, i, fix_add_vec, declear_fix_vec, declear_y_vec,
                    declear_add_vec);
  }
  genVHDLCodePerY(T, y_size, 0, fix_add_vec, declear_fix_vec, declear_y_vec,
                  declear_add_vec);

  genHeadAndStruct(y_size);
  genDeclareX(y_size);

  vector<string>::iterator vector_iterator;

  sort(declear_y_vec.begin(), declear_y_vec.end());
  vector_iterator = unique(declear_y_vec.begin(), declear_y_vec.end());
  if (vector_iterator != declear_y_vec.end()) {
    declear_y_vec.erase(vector_iterator, declear_y_vec.end());
  }
  for (int i = 0; i < declear_y_vec.size(); i++) {
    cout << "    " << declear_y_vec[i] << endl;
  }
  cout << endl;

  sort(declear_fix_vec.begin(), declear_fix_vec.end());
  vector_iterator = unique(declear_fix_vec.begin(), declear_fix_vec.end());
  if (vector_iterator != declear_fix_vec.end()) {
    declear_fix_vec.erase(vector_iterator, declear_fix_vec.end());
  }
  for (int i = 0; i < declear_fix_vec.size(); i++) {
    cout << "    " << declear_fix_vec[i] << endl;
  }
  cout << endl;

  sort(declear_add_vec.begin(), declear_add_vec.end());
  vector_iterator = unique(declear_add_vec.begin(), declear_add_vec.end());
  if (vector_iterator != declear_add_vec.end()) {
    declear_add_vec.erase(vector_iterator, declear_add_vec.end());
  }
  for (int i = 0; i < declear_add_vec.size(); i++) {
    cout << "    " << declear_add_vec[i] << endl;
  }

  cout << "begin" << endl;
  genREG(y_size);

  for (int i = 0; i < fix_add_vec.size(); i++) {
    cout << fix_add_vec[i] << endl;
  }
  cout << endl;

  cout << "output: y <= y01;" << endl << endl;
  cout << "end Behavioral;";
}

coeff_map_t readDictionary(string fileName) {
  coeff_map_t dictionary;
  map<int, int> variable_map;
  map<int, int> level_map;
  dictionary[1] = *option::addone();
  variable_map[0] = 1;
  level_map[1] = 0;

  ifstream fin;
  fin.open(fileName, ios::in);

  if (!fin.is_open()) {
    cout << "open File Failed." << endl;
  }

  string strOne;
  stringstream ss;
  while (getline(fin, strOne)) {
    bool bracket_first = true;
    int sign_bracket;
    int sign_equal;

    int r1, r2, key;
    for (int i = 0; i < strOne.length(); i++) {
      if (strOne[i] == '(') {
        sign_bracket = i + 1;
      } else if (strOne[i] == ')') {
        if (bracket_first) {
          ss.str(strOne.substr(sign_bracket, i - sign_bracket));
          ss >> key;
          ss.clear();
          bracket_first = false;
        } else {
          int sign_comma = strOne.find(",");

          if (strOne[sign_bracket] == 't') {
            ss.str(
                strOne.substr(sign_bracket + 1, sign_comma - sign_bracket - 1));
            ss >> r1;
            ss.clear();
            r1 = variable_map[r1];
          } else {
            ss.str(strOne.substr(sign_bracket, sign_comma - sign_bracket));
            ss >> r1;
            ss.clear();
          }

          if (strOne[sign_comma + 2] == 't') {
            ss.str(strOne.substr(sign_comma + 3, i - sign_comma - 3));
            ss >> r2;
            ss.clear();
            r2 = variable_map[r2];
          } else {
            ss.str(strOne.substr(sign_comma + 2, i - sign_comma - 2));
            ss >> r2;
            ss.clear();
          }
        }
      } else if (strOne[i] == '=') {
        sign_equal = i;
      }
    }

    int level = max(level_map[r1], level_map[r2]) + 1;
    switch (strOne[sign_equal + 3]) {
      case 'h':
        variable_map[key] = r1 << r2;
        level_map[r1 << r2] = level_map[r1];
        dictionary[r1 << r2] =
            *option::addshl(r1 << r2, 0, r1, r2, level_map[r1]);
        break;
      case 'd':
        variable_map[key] = r1 + r2;
        level_map[r1 + r2] = level;
        dictionary[r1 + r2] = *option::addshl(r1 + r2, r1, r2, 0, level);
        break;
      case 'u':
        variable_map[key] = r1 - r2;
        level_map[r1 - r2] = level;
        dictionary[r1 - r2] = *option::subshl(r1 - r2, r1, r2, 0, level);
        break;
      default:
        break;
    }
  }
  return dictionary;
}
option_vec_t getY(coeff_map_t dictionary, coeff_vec_t coeff_vec) {
  option_vec_t y;
  int index = 1;
  for (coeff_vec_t::iterator i = coeff_vec.begin(); i < coeff_vec.end(); i++) {
    bool isNeg = *i < 0;
    coeff_t temp_sh = 0;
    coeff_t temp_i = abs(*i);

    while ((temp_i & (coeff_t)1) == 0) {
      temp_i = (temp_i >> ((coeff_t)1));
      temp_sh++;
    }

    if (dictionary[temp_i].level > mlc) {
      mlc = dictionary[temp_i].level;
    }

    y.push_back(*option::mulshl(0, temp_i, isNeg ? -index : index, temp_sh));
    index++;
  }
  return y;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    cout << "Usage:\n" << endl;
    cout << "\t vhdl-gen [coefficient-file-path]";
    cout << " [acm-out-put-file-path] \n";
  }
  coeff_t coefficient;
  coeff_vec_t coeff_vec;
  coeff_map_t dictionary;

  string line;
  ifstream coef_file(argv[1]);
  if (coef_file.is_open()) {
    while (getline(coef_file, line)) {
      coeff_vec.push_back(stoll(line));
    }
  }

  cout << "Error open file:" << argv[1] << endl;
  // cout << "Please input the coefficient" << endl;
  //   while (cin >> coefficient) {
  //     coeff_vec.push_back(coefficient);
  //   }
  // dictionary = main_MCM(coeff_vec);
  dictionary = readDictionary(argv[2]);

  option_vec_t y = getY(dictionary, coeff_vec);

  coeff_map_vec_t T = CONV2CAVM(dictionary, mlc, y);

  // printT(T, y);
  genVHDLCode(T, y.size());

  return 0;
}
