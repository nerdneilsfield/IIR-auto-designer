#include "CAVM.hpp"

int index_T;

map<int, int> max_value;
void initialMaxMap()
{
	for (int i = 1; i <= 6; i++)
	{
		max_value[i] = -128;
		max_value[62 - i] = -128;
	}
	for (int i = 7; i <= 19; i++)
	{
		max_value[i] = 127;
		max_value[62 - i] = 127;
	}
	for (int i = 20; i <= 30; i++)
	{
		max_value[i] = -128;
		max_value[62 - i] = -128;
	}
	max_value[31] = -128;
}
int getX(int index)
{
	return index > 0 ? max_value[index] : -max_value[-index];
}
int calValue(coeff_map_vec_t T, option_vec_t y, int y_size)
{
	int result = 0;
	int r2;
	for (int i = 0; i < y.size(); i++)
	{
		if (abs(y[i].r2) > y_size)
		{
			if (y[i].r2 > 0)
			{
				r2 = calValue(T, T[y[i].r2], y_size);
			}
			else
			{
				r2 = -calValue(T, T[-y[i].r2], y_size);
			}			
		}
		else
		{
			r2 = getX(y[i].r2);
		}
		result += y[i].update(r2);
	}
	return result;
}
int calValue(coeff_map_vec_t T, option y_temp, int y_size)
{
	int r2;
	if (abs(y_temp.r2) > y_size)
	{
		if (y_temp.r2 > 0)
		{
			r2 = calValue(T, T[y_temp.r2], y_size);
		}
		else
		{
			r2 = -calValue(T, T[-y_temp.r2], y_size);
		}
	}
	else
	{
		r2 = getX(y_temp.r2);
	}
	return y_temp.update(r2);
}
void printT(coeff_map_vec_t T, option_vec_t y)
{
	int cost = -1;
	for (coeff_map_vec_t::iterator i = T.begin(); i != T.end(); i++)
	{
		cout << 'y' << i->first - y.size() << " = " << endl;
		for (option_vec_t::iterator j = i->second.begin(); j != i->second.end(); j++)
		{
			cout << "    ";
			if (j->r2 > 0)
			{
				if (j->r2 > y.size())
				{
					cout << j->r1 << " * y" << j->r2 - y.size() << " << " << j->sh << endl;
				}
				else
				{
					cout << j->r1 << " * x" << j->r2 << " << " << j->sh << endl;
				}
			}
			else
			{
				if (-(j->r2) > y.size())
				{
					cout << -j->r1 << " * y" << -(j->r2) - y.size() << " << " << j->sh << endl;
				}
				else
				{
					cout << -j->r1 << " * x" << -(j->r2) << " << " << j->sh << endl;
				}
			}
			cost++;
		}
	}
	cout << "Cost: " << cost << endl;
}
void factorAndEliminate(coeff_map_vec_t &T, option_vec_t &y)
{
	bool isDealed = false;
	option_vec_t y_res;
	for (int i = 0; i < y.size(); i++)
	{
		int temp_d = y[0].r1;

		for (int j = 1; j < y.size(); j++)
		{
			if (temp_d != 1 && temp_d == y[j].r1)
			{
				if (!isDealed)
				{
					T[index_T].push_back(*option::mulshl(0, 1, y[0].r2, y[0].sh));
					isDealed = true;
				}
				T[index_T].push_back(*option::mulshl(0, 1, y[j].r2, y[j].sh));
			}
			else
			{
				y_res.push_back(y[j]);
			}
		}
		if (!isDealed)
		{
			y_res.push_back(y[0]);
		}
		else
		{
			y_res.push_back(*option::mulshl(0, temp_d, index_T, 0));
			index_T++;
		}

		y = y_res;
		y_res.clear();
		isDealed = false;
	}
}

void substituteAndExpand(option_vec_t &y_res, coeff_map_t O, option yk)
{
	option temp_r1 = O[yk.r1];
	int r11, r12, sh1, r21, r22, sh2;

	if (O[temp_r1.r1].r1 == 0)
	{
		r11 = O[temp_r1.r1].r2;
		sh1 = yk.sh + O[temp_r1.r1].sh;
	}
	else
	{
		r11 = temp_r1.r1;
		sh1 = yk.sh;
	}

	if (O[temp_r1.r2].r1 == 0)
	{
		r21 = O[temp_r1.r2].r2;
		sh2 = yk.sh + temp_r1.sh + O[temp_r1.r2].sh;
	}
	else
	{
		r21 = temp_r1.r2;
		sh2 = yk.sh + temp_r1.sh;
	}

	if (temp_r1.type == option::ADDSHL)
	{
		y_res.push_back(*option::mulshl(0, r11, yk.r2, sh1));
		y_res.push_back(*option::mulshl(0, r21, yk.r2, sh2));
	}
	else if (yk.r1 == temp_r1.r1 - temp_r1.r2 << temp_r1.sh)
	{
		y_res.push_back(*option::mulshl(0, r11, yk.r2, sh1));
		y_res.push_back(*option::mulshl(0, r21, -yk.r2, sh2));
	}
	else
	{
		y_res.push_back(*option::mulshl(0, r11, -yk.r2, sh1));
		y_res.push_back(*option::mulshl(0, r21, yk.r2, sh2));
	}
}

coeff_map_vec_t CONV2CAVM(coeff_map_t O, int mlc, option_vec_t y)
{
	initialMaxMap();

	option_vec_t y_save = y;
	index_T = y_save.size() + 2;

	coeff_map_vec_t T; int step = mlc;
	

	cout << calValue(T, y, y_save.size()) << endl;

	factorAndEliminate(T, y);

	option_vec_t y_res;
	while (step > 0)
	{
		for (option_vec_t::iterator i = y.begin(); i != y.end(); i++)
		{

			int option_value = calValue(T, *i, y_save.size());
			int vector_befor = calValue(T, y_res, y_save.size());

			if (O[i->r1].level == step)
			{
				substituteAndExpand(y_res, O, *i);
			}
			else
			{
				y_res.push_back(*i);
			}

			int vector_after = calValue(T, y_res, y_save.size());
			if (vector_after != option_value + vector_befor)
			{
				cout << "option value: " << option_value << endl;
				cout << "vector befor: " << vector_befor << endl;
				cout << "vector after: " << vector_after << endl << endl;
			}

		}
		y = y_res;
		y_res.clear();
		factorAndEliminate(T, y);
		step--;
	}

	/*
	cout << "*************************************" << endl;
	printT(T, y_save);
	cout << "*************************************" << endl;
	*/

	/*
	for (int i = 1; i <= T.size(); i++)
	{
		option_vec_t y_temp = T[i + y_save.size() + 1];
		int option_value = calValue(T, y_temp, y_save.size());
		cout << "y" << i + 1 << ": " << option_value << endl;
	}
	*/

	/*
	int depth = 0;
	vector<option_vec_t> node_vec;
	vector<option_vec_t> node_vec_next;
	for (int i = 0; i < y.size(); i++)
	{
		option_vec_t temp;
		temp.push_back(y[i]);
		node_vec.push_back(temp);
	}
	while (node_vec.size() > 1)
	{
		bool single = node_vec.size() % 2 == 1;
		int node_vec_size = single ? node_vec.size() - 1 : node_vec.size();
		for (int j = 1; j <= node_vec_size; j++)
		{
			option_vec_t sig_A = node_vec[j - 1];
			j++;
			option_vec_t sig_B = node_vec[j - 1];

			option_vec_t temp_option_vec;
			for (int k = 0; k < sig_A.size(); k++)
			{
				temp_option_vec.push_back(sig_A[k]);
			}
			for (int k = 0; k < sig_B.size(); k++)
			{
				temp_option_vec.push_back(sig_B[k]);
			}

			cout << "sum_d" << to_string(depth) << "_i" << to_string(j / 2) << " = " << calValue(T, temp_option_vec, y_save.size()) << endl;

			node_vec_next.push_back(temp_option_vec);
		}
		if (single)
		{
			node_vec_next.push_back(node_vec.back());
		}
		node_vec = node_vec_next;
		node_vec_next.clear();
		depth++;
	}
	*/

	cout << calValue(T, y, y_save.size()) << endl;

	T[y_save.size()+1] = y;
	return T;
}