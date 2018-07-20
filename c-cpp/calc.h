/*   calc.h    */
#pragma once
#include <iostream>
#include <cstring>
#include <sstream>
#include <boost/regex.hpp>
#include <map>
#include <set>
#include <cmath>
//控制台颜色代码
#define blue 1
#define green 2
#define red 4
#define white 7
#define intense 8
#define Pi 3.1415926535897932
using namespace std;
//正则表达式
using boost::regex;
using boost::regex_match;
using boost::regex_replace;

inline void setc(int col) {//设置输出颜色
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
}

template<typename T, typename... U>
inline void print_(int col = white, T t = "", U... arg) {//彩色输出
	setc(col + intense);
	cout << t;
	setc(white);
	print_(col, arg...);
}
template<typename T>
inline void print_(int col = white, T t = "") {
	setc(col + intense);
	cout << t;
	setc(white);
}
inline void print_(int col = white, string t = "") {//cout输出string效率低，改用printf
	setc(col + intense);
	printf("%s", t.c_str());
	setc(white);
}

class err {//错误类，包含消息类型和消息
public:
	enum err_type { error, warning, tip }type;
	string msg;
	err(string s = "Error - Unable to exercute", err_type t = error) :msg(s), type(t) {}
};

void throw_(const char*s = "") {//快捷throw
	throw(err(string(s)));
}

class expression {//表达式类
public:
	vector<string> expr;
	friend ostream& operator<<(ostream& os, const expression& e) {
		os << "{";
		for (int i = 0, s = e.expr.size(); i <s; ++i) {
			cout << e.expr[i] << ",";
		}
		os << "}";
		return os;
	}
	expression() {}
	expression(const expression& e) {
		expr.assign(e.expr.begin(), e.expr.end());
	}
	expression(const vector<string>& s) {
		expr.assign(s.begin(), s.end());
	}
	expression(const string &s) {
		expr.push_back(s);
	}
	expression& operator~() {//运算符重载，用于翻转一个expression
		vector<string>t;
		t.insert(t.end(), expr.rbegin(), expr.rend());
		expr.swap(t);
		return *this;
	}
};

class func {//函数类
public:
	int var_num;//参数个数
	vector<string> var_list;//参数列表
	expression def;//表达式
	map<vector<string>, expression> partial1, partial2;//运算结果cache
	func(int n = 0, const vector<string>&vars = {}, const expression& expr = {}, const map<vector<string>, expression> &p1 = {}, const map<vector<string>, expression> &p2 = {}) :
		var_num(n), var_list(vars), def(expr), partial1(p1), partial2(p2) {}
	func(const func&f) : var_num(f.var_num), var_list(f.var_list), def(f.def), partial1(f.partial1), partial2(f.partial2) {}
};

class cal_stack {//栈单元类
public:
	vector<double>r_num;//数值栈
	expression numlist;//要计算的表达式
	bool re_ite = false;//是否返回
	string re_func;//要返回给的函数名
	vector<string> re_varlist;//要返回给的函数的参数列表
	cal_stack(const vector<double> &num_stack = {}, const expression &expr = {}, const string &f_name = "", const vector<string> &f_varlist = {}) :
		r_num(num_stack), numlist(expr), re_func(f_name), re_varlist(f_varlist), re_ite(f_name.size() > 0) {}
};

double randn(double x, double y) {//获取服从正态分布的随机数
	double u = 0.0, v = 0.0, w = 0.0, c = 0.0;
	do {
		u = rand() / (double)RAND_MAX * 2 - 1.0;
		v = rand() / (double)RAND_MAX * 2 - 1.0;
		w = u * u + v * v;
	} while (w == 0.0 || w >= 1.0);
	return u * sqrt((-2 * log(w)) / w)*x + y;
}

string last_warning, now_warning;
//内置变量定义
map<string, double> var_def = { { "pi",2 * asin(1) },{ "e",exp(1) },{ "inf",2 * 1e308 } }, var_def_init(var_def);
//内置函数定义
map<string, func> func_def = { { "out",{ 1 } } }, func_def_init(func_def);
map<string, double(*)(double)> func_in = { { "int",[](double x) {return floor(x); } },{ "sqrt",[](double x) {return sqrt(x); } },{ "ln",[](double x) {return log(x); } },{ "lg",[](double x) {return log10(x); } },{ "abs",[](double x) {return abs(x); } },{ "sin",[](double x) {return sin(x); } },{ "cos",[](double x) {return cos(x); } },{ "tan",[](double x) {return tan(x); } },{ "sinh",[](double x) {return sinh(x); } },{ "cosh",[](double x) {return cosh(x); } },{ "tanh",[](double x) {return tanh(x); } },{ "arcsin",[](double x) {return asin(x); } },{ "arccos",[](double x) {return acos(x); } },{ "arctan",[](double x) {return atan(x); } },{ "gamma",[](double x) {return tgamma(x); } },{ "erf",[](double x) {return erf(x); } } };
map<string, double(*)(double, double)> func_in2 = { { "max",[](double x,double y) {return x > y ? x : y; } },{ "min",[](double x,double y) {return x < y ? x : y; } },{ "log",[](double x,double y) {return log(x) / log(y); } },{ "C",[](double x,double y) {return tgamma(x + 1) / tgamma(x - y + 1) / tgamma(y + 1); } },{ "A",[](double x,double y) {return tgamma(x + 1) / tgamma(x - y + 1); } },{ "randn",randn } };
//编译好的正则表达式
const regex re_alp("^[a-zA-Z]+'?|'$"), re_num("^[\\+\\-]?[0-9\\.]+e?[\\+\\-]?[0-9\\.]*$"), plus1("([^\\w\\)])\\+"), minus1("([^\\w\\)])\\-"), plus2("^\\+"), minus2("^\\-"), num2("^[\\+\\-]?[0-9]+.*");
const string symbols = "+-*/^%\\(),=~`|!#";
//开关
map<string, bool> switches = { { "time",true },{ "fraction",true },{ "eng",true },{ "notime",false } };
//素数表
const int primes[] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,1051,1061,1063,1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,1153,1163,1171,1181,1187,1193,1201,1213,1217,1223,1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,1297,1301,1303,1307,1319,1321,1327,1361,1367,1373,1381,1399,1409,1423,1427,1429,1433,1439,1447,1451,1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,1523,1531,1543,1549,1553,1559,1567,1571,1579,1583,1597,1601,1607,1609,1613,1619,1621,1627,1637,1657,1663,1667,1669,1693,1697,1699,1709,1721,1723,1733,1741,1747,1753,1759,1777,1783,1787,1789,1801,1811,1823,1831,1847,1861,1867,1871,1873,1877,1879,1889,1901,1907,1913,1931,1933,1949,1951,1973,1979,1987,1993,1997,1999,2003,2011,2017,2027,2029,2039,2053,2063,2069,2081,2083,2087,2089,2099,2111,2113,2129,2131,2137,2141,2143,2153,2161,2179,2203,2207,2213,2221,2237,2239,2243,2251,2267,2269,2273,2281,2287,2293,2297,2309,2311,2333,2339,2341,2347,2351,2357,2371,2377,2381,2383,2389,2393,2399,2411,2417,2423,2437,2441,2447,2459,2467,2473,2477,2503,2521,2531,2539,2543,2549,2551,2557,2579,2591,2593,2609,2617,2621,2633,2647,2657,2659,2663,2671,2677,2683,2687,2689,2693,2699,2707,2711,2713,2719,2729,2731,2741,2749,2753,2767,2777,2789,2791,2797,2801,2803,2819,2833,2837,2843,2851,2857,2861,2879,2887,2897,2903,2909,2917,2927,2939,2953,2957,2963,2969,2971,2999,3001,3011,3019,3023,3037,3041,3049,3061,3067,3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,3181,3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,3259,3271,3299,3301,3307,3313,3319,3323,3329,3331,3343,3347,3359,3361,3371,3373,3389,3391,3407,3413,3433,3449,3457,3461,3463,3467,3469,3491,3499,3511,3517,3527,3529,3533,3539,3541,3547,3557,3559,3571,3581,3583,3593,3607,3613,3617,3623,3631,3637,3643,3659,3671,3673,3677,3691,3697,3701,3709,3719,3727,3733,3739,3761,3767,3769,3779,3793,3797,3803,3821,3823,3833,3847,3851,3853,3863,3877,3881,3889,3907,3911,3917,3919,3923,3929,3931,3943,3947,3967,3989,4001,4003,4007,4013,4019,4021,4027,4049,4051,4057,4073,4079,4091,4093,4099,4111,4127,4129,4133,4139,4153,4157,4159,4177,4201,4211,4217,4219,4229,4231,4241,4243,4253,4259,4261,4271,4273,4283,4289,4297,4327,4337,4339,4349,4357,4363,4373,4391,4397,4409,4421,4423,4441,4447,4451,4457,4463,4481,4483,4493,4507,4513,4517,4519,4523,4547,4549,4561,4567,4583,4591,4597,4603,4621,4637,4639,4643,4649,4651,4657,4663,4673,4679,4691,4703,4721,4723,4729,4733,4751,4759,4783,4787,4789,4793,4799,4801,4813,4817,4831,4861,4871,4877,4889,4903,4909,4919,4931,4933,4937,4943,4951,4957,4967,4969,4973,4987,4993,4999,5003,5009,5011,5021,5023,5039,5051,5059,5077,5081,5087,5099,5101,5107,5113,5119,5147,5153,5167,5171,5179,5189,5197,5209,5227,5231,5233,5237,5261,5273,5279,5281,5297,5303,5309,5323,5333,5347,5351,5381,5387,5393,5399,5407,5413,5417,5419,5431,5437,5441,5443,5449,5471,5477,5479,5483,5501,5503,5507,5519,5521,5527,5531,5557,5563,5569,5573,5581,5591,5623,5639,5641,5647,5651,5653,5657,5659,5669,5683,5689,5693,5701,5711,5717,5737,5741,5743,5749,5779,5783,5791,5801,5807,5813,5821,5827,5839,5843,5849,5851,5857,5861,5867,5869,5879,5881,5897,5903,5923,5927,5939,5953,5981,5987,6007,6011,6029,6037,6043,6047,6053,6067,6073,6079,6089,6091,6101,6113,6121,6131,6133,6143,6151,6163,6173,6197,6199,6203,6211,6217,6221,6229,6247,6257,6263,6269,6271,6277,6287,6299,6301,6311,6317,6323,6329,6337,6343,6353,6359,6361,6367,6373,6379,6389,6397,6421,6427,6449,6451,6469,6473,6481,6491,6521,6529,6547,6551,6553,6563,6569,6571,6577,6581,6599,6607,6619,6637,6653,6659,6661,6673,6679,6689,6691,6701,6703,6709,6719,6733,6737,6761,6763,6779,6781,6791,6793,6803,6823,6827,6829,6833,6841,6857,6863,6869,6871,6883,6899,6907,6911,6917,6947,6949,6959,6961,6967,6971,6977,6983,6991,6997,7001,7013,7019,7027,7039,7043,7057,7069,7079,7103,7109,7121,7127,7129,7151,7159,7177,7187,7193,7207,7211,7213,7219,7229,7237,7243,7247,7253,7283,7297,7307,7309,7321,7331,7333,7349,7351,7369,7393,7411,7417,7433,7451,7457,7459,7477,7481,7487,7489,7499,7507,7517,7523,7529,7537,7541,7547,7549,7559,7561,7573,7577,7583,7589,7591,7603,7607,7621,7639,7643,7649,7669,7673,7681,7687,7691,7699,7703,7717,7723,7727,7741,7753,7757,7759,7789,7793,7817,7823,7829,7841,7853,7867,7873,7877,7879,7883,7901,7907,7919,7927,7933,7937,7949,7951,7963,7993,8009,8011,8017,8039,8053,8059,8069,8081,8087,8089,8093,8101,8111,8117,8123,8147,8161,8167,8171,8179,8191,8209,8219,8221,8231,8233,8237,8243,8263,8269,8273,8287,8291,8293,8297,8311,8317,8329,8353,8363,8369,8377,8387,8389,8419,8423,8429,8431,8443,8447,8461,8467,8501,8513,8521,8527,8537,8539,8543,8563,8573,8581,8597,8599,8609,8623,8627,8629,8641,8647,8663,8669,8677,8681,8689,8693,8699,8707,8713,8719,8731,8737,8741,8747,8753,8761,8779,8783,8803,8807,8819,8821,8831,8837,8839,8849,8861,8863,8867,8887,8893,8923,8929,8933,8941,8951,8963,8969,8971,8999,9001,9007,9011,9013,9029,9041,9043,9049,9059,9067,9091,9103,9109,9127,9133,9137,9151,9157,9161,9173,9181,9187,9199,9203,9209,9221,9227,9239,9241,9257,9277,9281,9283,9293,9311,9319,9323,9337,9341,9343,9349,9371,9377,9391,9397,9403,9413,9419,9421,9431,9433,9437,9439,9461,9463,9467,9473,9479,9491,9497,9511,9521,9533,9539,9547,9551,9587,9601,9613,9619,9623,9629,9631,9643,9649,9661,9677,9679,9689,9697,9719,9721,9733,9739,9743,9749,9767,9769,9781,9787,9791,9803,9811,9817,9829,9833,9839,9851,9857,9859,9871,9883,9887,9901,9907,9923,9929,9931,9941,9949,9967,9973 };
//优先级表
map<char, int>prio = { { '(' , -1 },{ ')' , -2 },{ ',' , -3 },{ '+' , 1 },{ '-' , 1 },{ '~' , 1 },{ '`' , 1 },{ '*' , 2 },{ '/' , 2 },{ '%' , 2 },{ '\\' , 2 },{ '|' , 2 },{ '!' , 2 },{ '#',2 },{ '^' , 3 } };
//输出计数
int input_count = 0;
//迭代函数集合
set<string> func_ite;
ostringstream stream;


//判断对象是否含有某元素的统一方法
template<typename T>
inline bool has(const vector<T> &v, const T &s) {
	return find(v.begin(), v.end(), s) != v.end();
}
template<typename T, typename S>
inline bool has(const map<T, S> &v, const T &s) {
	return v.find(s) != v.end();
}
template<typename T>
inline bool has(const set<T> &v, const T &s) {
	return v.find(s) != v.end();
}
template<typename T>
inline bool has(const string &v, const T &s) {
	return v.find(s) != -1;
}
//符号类型
enum symtype { non, unknown, num, sci, sym, alp };
//判断符号类型
inline symtype s_type(const string &s) {
	//if (regex_match(s, re_alp)) return alp;
	if (s == "inf" || s == "-inf")return num;
	bool is_alp = true;
	for (int i = 0, j = s.size(); i < j; ++i) {
		if (!(('a' <= s[i] && 'z' >= s[i]) || ('A' <= s[i] && 'Z' >= s[i]) || s[i] == '\'')) {
			is_alp = false;
			break;
		}
		if (s[i] == '\''&&i != j - 1) {
			is_alp = false;
			break;
		}
	}
	if (is_alp)return alp;
	if (has(symbols, s[0])) return sym;
	if (regex_match(s, re_num))return num;
	/*bool is_poi = false, has_poi=false,is_e = false;
	int i, j;
	for (i = 0, j = s.size(); i < j; ++i) {
	if (s[i] == '+' || s[i] == '-'){
	if (i != 0 && s[i-1]!='e'){
	return unknown;
	}
	}
	if ('0' <= s[i] && '9' >= s[i]) is_poi = true;
	if (s[i] == '.'){
	if (!is_poi || has_poi) {
	return unknown;
	}
	has_poi = true;
	}
	if (s[i] == 'e') {
	if (is_e == true) {
	return unknown;
	}
	is_e = true;
	}
	}
	if (!(s[i - 1] >= '0'&&s[i - 1] <= '9')) {
	return unknown;
	}
	return num;*/
	return unknown;
}
inline symtype s_type(const char &s) {
	return s_type(string() + s);
}
template<typename T>
inline T pop(vector<T>&v) {//弹出最后一个元素并返回
	T tmp = v.back();
	v.pop_back();
	return tmp;
}

//转字符串的统一方法
template<typename T>
inline string toString(const T &s) {
	stream.str("");
	stream << s;
	return stream.str();
}
template<typename T>
vector<string> toString(const vector<T> &v) {
	vector<string> re;
	for (int i = 0, j = v.size(); i < j; ++i) {
		re.push_back(toString(v[i]));
	}
	return re;
}
//将string拆分
expression toArr(string str) {
	str = regex_replace(str, plus1, "$1~");
	str = regex_replace(str, minus1, "$1`");
	str = regex_replace(str, plus1, "$1~");
	str = regex_replace(str, minus1, "$1`");
	str = regex_replace(str, plus2, "~");
	str = regex_replace(str, minus2, "`");
	string last;
	int pos = -1;
	expression arr;
	symtype last_type = non, t;
	char i;
	for (int j = 0, len = str.length(); j < len; ++j) {
		i = str[j];
		++pos;
		t = s_type(i);
		if (t == unknown) {
			arr.expr.push_back(last);
			last = "";
			last_type = non;
			continue;
		}
		if (i == 'e'&&j>0 && s_type(str[j - 1]) == num && regex_match(str.substr(j + 1), num2)) {
			t = sci;
		}
		else if ((i == '+' || i == '-') && last_type == sci) {
			t = sci;
		}
		else if (t == num && last_type == sci) {
			;
		}
		else if (!(last_type == non || t == last_type) || t == sym) {
			arr.expr.push_back(last);
			last = "";
		}
		last += i;
		last_type = t;
	}
	arr.expr.push_back(last);
	vector<string>tmp;
	tmp.swap(arr.expr);
	for (int k = 0, s = tmp.size(); k < s; ++k) {
		if (tmp[k] != "")arr.expr.push_back(tmp[k]);
	}
	return arr;
}
inline expression toArr(char* s) {
	return toArr(string(s));
}
//转化为前缀表达式
expression toSuffix(const expression& e) {
	vector<string> nums, syms;
	string i, k, s;
	int p;
	int para_c = 0;
	for (int j = 0, len = e.expr.size(); j < len; ++j) {
		i = e.expr[j], s;
		if (i == "rnd") {
			i = toString(rand() / (double)RAND_MAX);
		}
		symtype t = s_type(i);
		if (t == num) {
			nums.push_back(i);
		}
		else if (t == alp) {
			k = i;
			k.pop_back();
			if (has(func_def, i) || has(func_in, i) || has(func_in2, i)) {
				syms.push_back(i);
			}
			else if (i.back() == '\'' && (has(func_def, k) || has(func_in, k) || has(func_in2, k))) {
				syms.push_back(i);
			}
			else {
				nums.push_back(i);
			}
		}
		else if (i == "`" || i == "~" || i == "!") {
			syms.push_back(i);
		}
		else if (t == sym) {
			p = prio[i[0]];
			if (i.length() != 1)p = 100;//无效数字
			if (p == -1) {
				syms.push_back(i);
				para_c++;
			}
			else if (p == -2) {
				para_c--;
				if (para_c < 0) {
					throw_("Error - Unmatched Parathesis");
				}
				while (!((s = pop(syms)) == "(")) {//删除左括号
					nums.push_back(s);
				}
				if (syms.size() > 0) {
					s = syms.back();
					if (s_type(s) == alp) {
						nums.push_back(pop(syms));
					}
				}
			}
			else if (p == -3) {//保留左括号
				while (!((s = syms.back()) == "(")) {
					nums.push_back(pop(syms));
				}
			}
			else {
				while (syms.size() > 0) {
					s = syms.back();
					if (s.length() > 1)break;
					int ps = prio[s[0]];
					if ((ps > p&&p == 3) || (ps >= p && p != 3)) {
						nums.push_back(s);
						syms.pop_back();
					}
					else {
						break;
					}
				}
				syms.push_back(i);
			}
		}
	}
	nums.insert(nums.end(), syms.rbegin(), syms.rend());
	return nums;
}
//带错误处理的string转double
inline double to_num(const string &s) {
	try {
		return stod(s);
	}
	catch (...) {
		throw(err(string("Error - [") + s + "] is not of type double"));
	}
}
//表达式计算
double cal(const expression &arr,bool is_suffix=false) {
	expression nums;
	if (is_suffix) {
		nums = arr;
	}
	else {
		nums = toSuffix(arr);
	}
	vector<cal_stack> cal_queue = { { {},~nums } };
	double re;
	string k;
	while (cal_queue.size() > 0) {
		bool next_queue = false;
		cal_stack cs = pop(cal_queue);
		auto &numlist = cs.numlist.expr;
		while (numlist.size() > 0) {
			k = pop(numlist);
			symtype t = s_type(k);
			double tmp;
			if (t == num) {
				tmp = to_num(k);
				cs.r_num.push_back(tmp);
			}
			else if (k == "`" || k == "!" || k == "~") {
				if (cs.r_num.size() < 1) {
					throw_("Error - Insufficient numbers");
				}
				if (k == "`") {
					cs.r_num.push_back(-pop(cs.r_num));
				}
				else if (k == "!") {
					tmp = tgamma(pop(cs.r_num) + 1);
					if (isnan(tmp)) throw_("Error - Not a number");
					cs.r_num.push_back(tmp);
				}
			}
			else if (t == sym) {
				if (cs.r_num.size() < 2) {
					throw_("Error - Insufficient numbers");
				}
				double a, b;
				b = pop(cs.r_num);
				a = pop(cs.r_num);
				if (k == "+") {
					tmp = a + b;
				}
				else if (k == "-") {
					tmp = a - b;
				}
				else if (k == "*") {
					tmp = a * b;
				}
				else if (k == "/") {
					tmp = a / b;
				}
				else if (k == "%") {
					tmp = fmod(a, b);
				}
				else if (k == "^") {
					tmp = pow(a, b);
				}
				else if (k == "\\") {
					tmp = (a - fmod(a, b)) / b;
				}
				else if (k == "|") {
					tmp = a * b / (a + b);
				}
				else if (k == "#") {
					if (a < 0 || b < 0) {
						throw_();
					}
					for (int i = 0; i < 50; ++i) {
						tmp = (a + b) / 2;
						b = sqrt(a)*sqrt(b);
						a = tmp;
					}
					tmp = a;
				}
				if (isnan(tmp)) throw_("Error - Not a number");
				cs.r_num.push_back(tmp);
			}
			else if (t == alp) {
				if (has(var_def, k)) {
					cs.r_num.push_back(var_def[k]);
				}
				else {
					auto tmp = k;
					tmp.pop_back();
					if (has(func_def, k) || has(func_in, k) || has(func_in2, k) || (k.back() == '\'' && (has(func_def, tmp) || has(func_in, tmp) || has(func_in2, tmp)))) {
						double f_re;
						if (k.back() == '\'') {//求导
							if (cs.r_num.size() < 1) {
								throw_("Error - Insufficient numbers");
							}
							double x = pop(cs.r_num);
							k.pop_back();
							f_re = cal({ { "(",k,"(",toString(x),"+","1e-8",")","-",k,"(",toString(x),")",")","/","1e-8" } });
						}
						else {
							if (has(func_in, k)) {
								if (cs.r_num.size() < 1) {
									throw_("Error - Insufficient numbers");
								}
								f_re = func_in[k](pop(cs.r_num));
							}
							else if (has(func_in2, k)) {
								if (cs.r_num.size() < 2) {
									throw_("Error - Insufficient numbers");
								}
								double y = pop(cs.r_num);
								double x = pop(cs.r_num);
								f_re = func_in2[k](x, y);
							}
							else {
								func &f_list = func_def[k];
								map<string, double> f_def;
								expression f_exp;
								int f_len = f_list.var_num;
								vector<string> f_match;
								expression f_matchexp;
								vector<double> f_num;
								if (cs.r_num.size() < f_len) {
									throw_("Error - Insufficient numbers");
								}
								f_num.insert(f_num.end(), cs.r_num.end() - f_len, cs.r_num.end());
								if (k == "out") {
									if (f_num[0] < 0) {
										f_num[0] = input_count + f_num[0];
									}
								}
								vector<string>t;
								if ((has(func_ite, k) || k == "out") && has(f_list.partial1, t = toString(f_num))) {
									f_match = t;
									f_matchexp = f_list.partial1[f_match];
								}
								else {
									for (auto key = f_list.partial2.begin(), e = f_list.partial2.end(); key != e; ++key) {
										bool matched = true;
										auto i = key->first;
										for (int j = 0; j < f_len; ++j) {
											if (!(s_type(i[j]) == alp || i[j] == toString(f_num[j]))) {
												matched = false;
											}
										}
										if (matched) {
											f_match = i;
											f_matchexp = f_list.partial2[i];
										}
									}
								}
								bool noPartial = (f_match.size() == 0);
								if (noPartial) {
									f_match = f_list.var_list;
									f_matchexp = f_list.def;
								}
								if (f_match.size() == 0) {
									throw_("Error - Undefined");
								}
								for (int i = 0; i < f_len; ++i) {
									string f_var = f_match[f_match.size() - 1 - i];
									double p = pop(cs.r_num);
									if (s_type(f_var) == alp) {
										f_def[f_var] = p;
									}
								}
								for (auto j = f_matchexp.expr.begin(), e = f_matchexp.expr.end(); j != e; ++j) {
									string i = *j;
									if (s_type(i) == alp && has(f_def, i)) {
										i = toString(f_def[i]);
									}
									f_exp.expr.push_back(i);
								}
								if (has(func_ite, k) && noPartial) {
									cal_queue.push_back({ cs.r_num, numlist });
									cal_queue.push_back({ {}, ~f_exp, k, toString(f_num) });
									next_queue = true;
									break;
								}
								else {
									f_re = cal(f_exp, true);
								}
							}
						}
						if (isnan(f_re)) throw_("Error - Not a number");
						cs.r_num.push_back(f_re);
					}
					else {
						now_warning = "Warning - [" + k + "] Undefined, taken as 0";
						if (now_warning != last_warning) {
							print_(red + green, now_warning, '\n');
							last_warning = now_warning;
						}
						cs.r_num.push_back(0);
					}
				}
			}
		}
		if (next_queue) {
			continue;
		}
		if (cs.r_num.size() != 1) {
			throw_("Error - Too many numbers");
		}
		if (cal_queue.size() > 0) {
			cal_queue.back().r_num.push_back(cs.r_num[0]);
			if (cs.re_ite) {
				func_def[cs.re_func].partial1[cs.re_varlist] = { toString(cs.r_num[0]) };
			}
		}
		re = cs.r_num[0];
	}
	if (isnan(re)) throw_("Error - Not a number");
	return re;
}
//拼写改正
string& intelliSpell(string &st) {
	if (switches["eng"]) {
		char a[] = "TGMk.munpf";
		for (int i = 15; i > -16; i -= 3) {
			char eng = a[i];
			if (eng == '.') {
				continue;
			}
			st = regex_replace(st, regex(string("\\d") + eng + "[^0-9]*"), "e" + toString(i));
		}
	}
	st = regex_replace(st, regex("(\\d)e\\+(\\d)"), "$1@$2");
	st = regex_replace(st, regex("(\\d)e([\\-0]*\\d)"), "$1@$2");
	st = regex_replace(st, regex("(\\d)([a-zA-Z\\(])"), "$1*$2");
	st = regex_replace(st, regex("([a-zA-Z])(\\d)"), "$1*$2");
	st = regex_replace(st, regex("(\\))([a-zA-Z\\d])"), "$1*$2");
	st = regex_replace(st, regex("(\\))(\\()"), "$1*$2");
	st = regex_replace(st, regex("@"), "e");
	return st;
}
inline double calc_(const string &st) {
	return cal(toArr(st));
}
vector<string> split(const string& str, char token) {//以token分割string，返回vector
	string tmp;
	vector<string> v;
	for (int i = 0, j = str.size(); i < j; ++i) {
		if (str[i] != token) {
			tmp.push_back(str[i]);
		}
		else {
			if (tmp.size() > 0) {
				v.push_back(tmp);
				tmp = "";
			}
		}
	}
	if (tmp.size() > 0) {
		v.push_back(tmp);
	}
	return v;
}

string equ_solve(vector<string> arr) {//解方程
	if (arr.size() < 1) {
		throw_();
	}
	auto f = arr[0];
	double c = calc_(arr[1]);
	if ((!has(func_def, f) || func_def[f].var_num != 1) && !has(func_in, f)) {
		throw(err("Error - Function [" + f + "] with exactly 1 variable is required!"));
	}
	double equ_delta = 1e-9, equ_preci = 1e-9, x__ = 0.271828;
	int equ_ite = 100000;
	if (arr.size() == 3) {
		x__ = calc_(arr[2]);
	}
	double y, y_, y__;
	bool found = false;
	for (int i = 0; i < equ_ite; ++i) {
		try {
			y = cal({ { f,"(",toString(x__),")" } });
			y_ = cal({ { f,"(",toString(x__ + equ_delta),")" } });
			y__ = cal({ { f,"(",toString(x__ - equ_delta),")" } });
			if (y_ == y__) {
				throw_();
			}
		}
		catch (...) {
			x__ += 3.14159;
		}
		double x_ = x__ - (y - c) / (y_ - y__) * 2 * equ_delta;
		if (abs(x_ - x__) < equ_preci) {
			x__ = x_;
			break;
			found = true;
		}
		x__ = x_;
	}
	if (!found) {
		print_(red + green, "Warning:Itenaration Limit Exceded!\nThe result might be inaccurate.\n");
	}
	return toString(x__);
}

string func_sum(const vector<string> &arr) {//函数求和
	if (arr.size() < 3) {
		throw(true);
	}
	auto f = arr[0];
	if ((!has(func_def, f) || func_def[f].var_num != 1) && !has(func_in, f)) {
		throw(err("Error - Function [" + f + "] with exactly 1 variable is required!"));
	}
	double start = calc_(arr[1]), start_ = start, end = calc_(arr[2]), exp_re = 0;
	double step = arr.size() >= 4 ? calc_(arr[3]) : 1.0;
	if ((end - start)*step < 0) {
		throw_();
	}
	int s = (int)floor(step / abs(step));
	while ((end - start)*s >= 0) {
		exp_re += cal({ { f,"(",toString(start),")" } });
		start += step;
	}
	print_(green + blue, "From ", start_, " to ", start - step, '\n');
	return toString(exp_re);
}
string func_mlt(const vector<string> &arr) {//求积
	if (arr.size() < 3) {
		throw(true);
	}
	auto f = arr[0];
	if ((!has(func_def, f) || func_def[f].var_num != 1) && !has(func_in, f)) {
		throw(err("Error - Function [" + f + "] with exactly 1 variable is required!"));
	}
	double start = calc_(arr[1]), start_ = start, end = calc_(arr[2]), exp_re = 0;
	double step = arr.size() >= 4 ? calc_(arr[3]) : 1.0;
	if ((end - start)*step < 0) {
		throw_();
	}
	while ((end - start)*step >= 0) {
		exp_re *= cal({ { f,"(",toString(start),")" } });
		start += step;
	}
	print_(green + blue, "From ", start_, " to ", start - step, '\n');
	return toString(exp_re);
}
string func_list(const vector<string>&arr) {//函数列表，为提高效率用printf代替cout
											//$list sin+cos+ln -pi pi
											//$list sin+cos 0 2pi 0.1
	const int width = 18;
	if (arr.size() < 3) {
		throw_();
	}
	auto fs = split(arr[0], '+');
	for (int i = 0; i < fs.size(); ++i) {
		if ((!has(func_def, fs[i]) || func_def[fs[i]].var_num != 1) && !has(func_in, fs[i])) {
			throw(err("Error - Function [" + fs[i] + "] with exactly 1 variable is required!"));
		}
	}
	double start = calc_(arr[1]), end = calc_(arr[2]);
	double step = arr.size() >= 4 ? calc_(arr[3]) : 1.0;
	if ((end - start)*step < 0) {
		throw_();
	}
	setc(white + intense);
	for (int i = 0, j = (fs.size() + 1); i < j; ++i) {
		printf("___________________");
	}
	printf("\n%*s", width + 1, "x|");
	for (int i = 0; i < fs.size(); ++i) {
		printf("%*s|", width, fs[i].c_str());
	}
	printf("\n");
	for (int i = 0; i <= fs.size(); ++i) {
		printf("__________________|");
	}
	printf("\n");
	while ((end - start)*step >= 0) {
		printf("%*s|", width, toString(start).c_str());
		for (int i = 0; i < fs.size(); ++i) {
			try {
				printf("%*s", width, toString(cal({ { fs[i],"(",toString(start),")" } })).c_str());
			}
			catch (...) {
				setc(red + intense);
				printf("%*s", width, "Error");
				setc(white + intense);
			}
			printf("|");
		}
		printf("\n");
		start += step;
	}
	for (int i = 0, j = (fs.size() + 1)*(width + 1); i < j; ++i) {
		printf("~");
	}
	printf("\n");
	return "";
}
string func_integ(const vector<string>&arr) {//函数积分
	if (arr.size() < 3) {
		throw_();
	}
	auto f = arr[0];
	if ((!has(func_def, f) || func_def[f].var_num != 1) && !has(func_in, f)) {
		throw(err("Error - Function [" + f + "] with exactly 1 variable is required!"));
	}
	double start = calc_(arr[1]), end = calc_(arr[2]);
	int step = arr.size() >= 4 ? (int)calc_(arr[3]) : 1000;
	step *= 2;
	double delta = (end - start) / step, result = 0;
	bool warning_ = false;
	for (int i = 0; i < step + 1; ++i) {
		double exp_re;
		try {
			exp_re = cal({ { f,"(",toString(start + i * delta),")" } });
			if (i == 0 || i == step + 1) {
				result += exp_re;
			}
			else if (i & 1) {
				result += 4 * exp_re;
			}
			else {
				result += 2 * exp_re;
			}
		}
		catch (...) {
			warning_ = true;
		}
	}
	result *= delta / 3;
	if (warning_) {
		print_(red + green, "Error detected, the result maybe inaccurate:\n");
	}
	return toString(result);
}
