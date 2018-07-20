/*   app.cpp   */
#include <iostream>
#include <cstring>
#include <signal.h>
#include <fstream>
#include <time.h>
#include <Windows.h>
#include "calc.h"
#include "2048.h"

using namespace std;
using boost::regex;//正则表达式
using boost::regex_match;//正则匹配
using boost::regex_replace;//正则替换
//g(x)=g(x-1)+g(x-2);g(1)=1;g(2)=1;g(1000) 0.037s
//g(x)=g(x-1)+g(x-2);g(1)=1;g(2)=1;$list g 1 100

const string help = "                  Calc Pro by John Ao\n                  ~~~~~~~~~~~~~~~~~~~\n  本程序支持实数域表达式运算、变量和函数定义、方程求解，\n  函数列表、求和、求积、积分、求导，分解质因数等功能，\n  多行输入可用半角分号';'隔开。\n\n·表达式运算\n\n    ·忽略空白符\n\n    ·按 Ctrl+C 中断操作或放弃当前输入行\n\n    ·支持一元运算符 + - ! 和二元运算符 + - * / \\ % ^ | #\n            (正、负、阶乘)(加、减、乘、除、整除、取余、求幂、并联电阻、AGM)\n\n    ·数字和常数、变量、函数之间的乘号可以省略\n\n    ·支持科学计数法，如 1e3, 1e-3\n\n    ·内置基础函数\n        ·一元函数 int,sqrt,ln,lg,abs,sin,cos,tan,sinh,cosh,tanh,arcsin,arccos,arctan,gamma,erf\n        ·二元函数 max,min,log,C,A,randn\n        ·其中int为下取整，C(m,n)计算组合数，A(m,n)计算排列数，\n         log(m,n)计算log_m(n)，randn(μ,σ^2)获取服从正态分布N(μ,σ^2)的随机数。\n\n    ·内置函数out(n)存储第n次输出的结果（n可以为负）\n\n    ·内置常量有 pi,e\n\n    示例：\n    1) 3+4*2/(1-5)^2^3-C(5,2)max(-pi,e)+sin'(2pi)\n    2) 3|(4+2|(5+3|1))+4\n\n·变量和函数的定义及使用\n\n    ·变量名、函数名和函数的自变量名 只能是a-z和A-Z的组合\n\n    · 变量名和函数名不能相同\n\n    ·用'='符号表示定义，若为变量定义，'='后的表达式会被 立即计算并赋值给变量\n\n    ·递归函数的初值定义方式与变量定义相同\n\n    ·变量或函数可参与表达式运算\n\n    ·变量rnd为[0,1)上 一致分布的随机数\n     获取服从 正态分布N(μ,σ^2)的随机数使用函数randn(μ,σ^2)\n\n    ·清除某一定义输入 $clean 变量名或函数名\n     重新定义函数会自动清除原函数的定义\n\n    ·清除所有定义输入 $clean\n\n    示例：\n    1) abc=1+2/3; 4+abc\n       定义变量abc并使用\n\n    2) f(ab,bc)=(ab+bc)/2; f(3,4)\n       定义二元函数f(ab,bc)并使用\n\n    3) $clean g;g(x)=g(x-1)+g(x-2); g(1)=1; g(2)=1; g(2000)\n       清除g的定义，重新定义g(x)为Fibonacci数列并求第2000项\n\n·方程求解 - 牛顿法解方程\n\n    ·(以x0为初值)求解形如 f(x)=A 的问题\n\n     $solve f A x0\n\n    ·x0为可选参数\n\n    示例：\n    1) g(y)=ln(y); $solve g 1 2.7\n       以2.7为初值求解方程ln(y)=1\n\n    2) f(x)=max(sin(x/2),x); $solve f -1 0.4\n       以0.4为初值求解方程max(sin(x/2),x)=-1\n\n·函数列表、求和、求积\n\n    ·从a到b以步长step列表、求和、求积 （省略step则默认为1）\n        ·$list f a b step\n        ·$sum f a b step\n        ·$product f a b step\n\n    ·同时对多个函数列表用'+'或','连接\n\n    示例：\n    1) f(x)=1-1/(2x)^2;$product f 1 1000;2/out(-1)\n        ∞               2\n        ∏ (1-1/(2*x)^2)=—\n        x=1              π\n       求f(x)从1到1000的部分积，并近似计算π的值\n\n    2) f(x)=2^x;g(x)=x^2;$list f+g 0 5\n       从0到5对f(x)和g(x)列表\n\n    3) g(x)=-(-1)^x/(2x-1);$sum g 1 100;ln(2)-out(-1)\n        ∞\n        ∑ -(-1)^x/(2*x-1)=ln(2)\n        x=1\n       求g(x)从1到100的部分和，并近似计算ln2的值\n\n·函数积分\n    ·对一元函数f(x)，从a到b积分，取p份分割 （省略p则默认为1000）\n     $integrate f a b p\n\n·函数求导\n    ·求一元函数f(x)在a处的近似导数，使用f'(a)\n    示例:\n    1) sin'(pi/6)\n\n·运算计时\n\n    ·输入 $time 开启/关闭计时功能\n\n·分数输出\n\n    ·输入 $fraction 开启/关闭按分数格式输出功能\n     注意：由于精度限制，在一些极端情况下结果可能不准确\n\n    示例：\n    1) $fraction$;sqrt(4/5)pi;arctan(1)\n\n·2048\n    连续输入两次2048可开启彩蛋\n\n·退出程序\n    $exit\n\n";
static volatile bool broken = false;

void break_handler(int sig) {
	if (sig == SIGINT || sig == SIGBREAK) {
		broken = true;
		cin.clear();
		cin.sync();
		print_(red, "\nAborted.\n");
	}
	signal(SIGINT, break_handler);
	signal(SIGBREAK, break_handler);
}

void randomHead() {//随机LOGO
	vector<string> h = { "  ___    __    __    ___    ____  ____  _____ \n / __)  /__\\  (  )  / __)  (  _ \\(  _ \\(  _  )\n( (__  /(__)\\  )(__( (__    )___/ )   / )(_)( \n \\___)(__)(__)(____)\\___)  (__)  (_)\\_)(_____)\n",
		"_________        .__           __________                \n\\_   ___ \\_____  |  |   ____   \\______   \\_______  ____  \n/    \\  \\/\\__  \\ |  | _/ ___\\   |     ___/\\_  __ \\/  _ \\ \n\\     \\____/ __ \\|  |_\\  \\___   |    |     |  | \\(  <_> )\n \\______  (____  /____/\\___  >  |____|     |__|   \\____/ \n        \\/     \\/          \\/                            \n",
		" _______  _______  ___      _______    _______  ______    _______ \n|       ||   _   ||   |    |       |  |       ||    _ |  |       |\n|       ||  |_|  ||   |    |       |  |    _  ||   | ||  |   _   |\n|       ||       ||   |    |       |  |   |_| ||   |_||_ |  | |  |\n|      _||       ||   |___ |      _|  |    ___||    __  ||  |_|  |\n|     |_ |   _   ||       ||     |_   |   |    |   |  | ||       |\n|_______||__| |__||_______||_______|  |___|    |___|  |_||_______|\n",
		"   ______      __        ____           \n  / ____/___ _/ /____   / __ \\_________ \n / /   / __ `/ / ___/  / /_/ / ___/ __ \\ \n/ /___/ /_/ / / /__   / ____/ /  / /_/ /\n\\____/\\__,_/_/\\___/  /_/   /_/   \\____/ \n                                        \n" };
	print_(green + blue, h[rand() % 4]);
}

inline bool is_int(double num, double preci = 1e-7) {//判断在精度范围内是否是整数
	double t = int(num + 0.1) - num;
	return -preci <= t && t <= preci;
}

void script(int, char*[]);				//用来处理脚本文件
map<int, int>factor(int);				//分解质因数
int pow(int, int);						//整数快速幂
string prettyPrint(double num);		//数学输出
string exec(string&, bool&, bool);		//执行语句
void script(int, char*[]);				//执行脚本

map<int, int> factor(int num) {
	if (num == 0) {
		return{ { 0,1 } };
	}
	map<int, int> fact = {};
	bool found = false;
	for (int i = 0; i < 1229; i++) {
		int p = primes[i];
		while (num%p == 0) {
			num /= p;
			if (has(fact, p)) {
				fact[p] += 1;
			}
			else {
				fact[p] = 1;
			}
		}
		if (num == 1) {
			found = true;
			break;
		}
	}
	if (!found) {
		fact[num] = 1;
	}
	return fact;
}
int pow(int x, int y) {
	int re = 1;
	while (y != 0) {
		if (y & 1) {
			re *= x;
		}
		y >>= 1;
		x *= x;
	}
	return re;
}
string prettyPrint(double num) {
	if (num > 1e9 || num < 1e-4) {
		return "";
	}
	string re;
	double q;
	int p;
	for (p = 1; p < 10000; ++p) {
		q = num * p;
		double q_ = q;
		if (is_int(q)) {
			re = toString(int(q + 0.1));
			if (p == 1) {
				return re;
			}
			else {
				return re + '/' + toString(p);
			}
		}
		q = q_ / Pi;
		if (is_int(q)) {
			re = toString(int(q + 0.1));
			if (re == "1") {
				re = "π";
			}
			else {
				re += "π";
			}
			if (p == 1) {
				return re;
			}
			else {
				return re + '/' + toString(p);
			}
		}
		q = q_ * Pi;
		if (is_int(q)) {
			if (p == 1) {
				re = "π";
			}
			else {
				re = "(" + toString(p) + "π)";
			}
			return toString(int(q + 0.1)) + "/" + re;
		}
	}
	double num_pi = (num / Pi), num_ = num * num;
	num_pi *= num_pi;
	bool flag_pi = false, found = false;
	for (p = 1; p < 10000; ++p) {
		q = num_ * p;
		if (is_int(q)) {
			found = true;
			break;
		}
		q = num_pi * p;
		if (is_int(q)) {
			flag_pi = true;
			break;
		}
	}
	if (!(flag_pi | found)) {
		return "";
	}
	auto p_f = factor(p);
	auto q_f = factor(int(q + 0.1));
	int p_out = 1;
	for (auto i = p_f.begin(), j = p_f.end(); i != j; ++i) {
		int x = i->first, y = i->second;
		if (y & 1) {
			y += 1;
			if (has(q_f, x)) {
				q_f[x] += 1;
			}
			else {
				q_f[x] = 1;
			}
		}
		p_out *= pow(x, y / 2);
	}
	int q_out = 1, q_in = 1;
	for (auto i = q_f.begin(), j = q_f.end(); i != j; ++i) {
		int x = i->first, y = i->second;
		if (y & 1) {
			q_in *= x;
			--y;
		}
		q_out *= pow(x, y / 2);
	}
	string q_in_ = "√" + toString(q_in), qq;
	if (q_out == 1) {
		qq = q_in_;
	}
	else {
		qq = toString(q_out) + q_in_;
	}
	if (flag_pi) {
		qq += "π";
	}
	if (p_out == 1) {
		return qq;
	}
	else {
		return qq + '/' + toString(p_out);
	}
}
string exec(string &exp_in,bool &is_exp_re,bool is_script=false) {
	last_warning = "";//保存上一次警告内容
	string exp_re;
	if (regex_match(exp_in, regex("^\\s*\\$.*"))) {//处理$号打头的指令
		exp_in = intelliSpell(exp_in);//自动更正
		if (!is_script) {
			print_(green, "In[", input_count, "]:=");
			print_(white, exp_in, '\n');
		}
		auto exp_spec = split(exp_in, ' ');//分割输入
		string cmd = exp_spec[0];
		cmd = cmd.substr(1);
		exp_spec.assign(exp_spec.begin() + 1, exp_spec.end());
		//判断各种指令
		if (cmd == "solve") {
			exp_re = equ_solve(exp_spec);
			is_exp_re = true;
		}
		else if (cmd == "sum") {
			exp_re = func_sum(exp_spec);
			is_exp_re = true;
		}
		else if (cmd == "list") {
			exp_re = func_list(exp_spec);
		}
		else if (cmd == "product") {
			exp_re = func_mlt(exp_spec);
			is_exp_re = true;
		}
		else if (cmd == "integrate") {
			exp_re = func_integ(exp_spec);
			is_exp_re = true;
		}
		else if (cmd == "exit") {
			exit(0);
		}
		else if (cmd == "exec") {
			int arg = exp_spec.size();
			char** args = new char*[arg];
			for (int i = 0; i < arg; ++i) {
				args[i+1] = new char[exp_spec[i].size() + 1];
				memcpy(args[i+1], exp_spec[i].c_str(),exp_spec[i].size()+1);
			}
			script(arg+1, args);
			for (int i = 0; i < arg; ++i) {
				delete[] args[i];
			}
			delete[] args;
		}
		else if (cmd == "clean") {
			if (exp_spec.size() > 0 && s_type(exp_spec[0]) == alp) {
				if (has(func_def, exp_spec[0]) && !(has(func_in, exp_spec[0]) || has(func_in2, exp_spec[0]))) {
					func_def.erase(exp_spec[0]);
					func_ite.erase(exp_spec[0]);
				}
				if (has(var_def, exp_spec[0])) {
					var_def.erase(exp_spec[0]);
				}
			}
			else {
				var_def = var_def_init;
				func_def = func_def_init;
			}
		}
		else if (has(switches, cmd)) {//处理开关类指令
			if (switches[cmd] = !switches[cmd]) {
				exp_re = cmd + " on";
			}
			else {
				exp_re = cmd + " off";
			}
		}
		else if (has(switches, exp_re = cmd.substr(0, cmd.size() - 1))) {
			switches[exp_re] = true;
			exp_re += " on";
		}
		else if (has(switches, exp_re = cmd.substr(0, cmd.size() - 2))) {
			switches[exp_re] = false;
			exp_re += " off";
		}
		else {
			exp_re = help;
		}
	}
	else {//表达式计算
		exp_in = intelliSpell(regex_replace(exp_in, regex("\\s+"), ""));
		if (!is_script) {
			print_(green, "In[", input_count, "]:=");
			print_(white, exp_in, '\n');
		}
		auto exp_arr = toArr(exp_in).expr;
		if (has(exp_arr, string("="))) {//判断是否为赋值
			int equ_pos = find(exp_arr.begin(), exp_arr.end(), string("=")) - exp_arr.begin();//等于号的位置
			if (regex_match(exp_in, regex("^[a-zA-Z]+=.+"))) {//变量定义
				if (has(func_def, exp_arr[0]) || has(func_in, exp_arr[0]) || has(func_in2, exp_arr[0])) {
					throw(err());
				}
				expression e;
				e.expr.assign(exp_arr.begin() + 2, exp_arr.end());
				var_def[exp_arr[0]] = cal(e);//存储变量值
				exp_re = toString(var_def[exp_arr[0]]);
			}
			else if (regex_match(exp_in, regex("^\\w+\\([\\w,]*\\w+\\)\\=.+"))) {//函数定义
				string f = exp_arr[0];
				vector<string> f_var;
				for (int i = 2; i < equ_pos - 1; i += 2) {//函数参数
					f_var.push_back(exp_arr[i]);
				}
				bool f_isdef = false;//标记是否为定义
				expression f_exp;
				f_exp.expr.assign(exp_arr.begin() + equ_pos + 1, exp_arr.end());//函数表达式
				if (!has(func_def, f)) {//加入f，否则toSuffix出错
					func_def[f] = {};
				}
				f_exp=toSuffix(f_exp);
				bool f_var_allnum = true;//标记是否全为数值
				for (int j = 0; j < f_var.size(); ++j) {
					string i = f_var[j];
					if (s_type(i) == num) {
						f_isdef = true;
					}
					else {
						f_var_allnum = false;
					}
				}
				if (has(func_in, f)) {//判断是否内置函数
					throw(err(string("Error - Function [") + f + "] Defined Inside!"));
				}
				else if (f == "out") {
					print_(red, "Error - Function [out] is reserved!\n");
					print_(blue + green, "->Try $help for details.\n");
				}
				else {
					if (has(func_def, f)) {
						if (f_isdef) {
							if (!has(func_ite, f)) {//非递归函数不可赋值
								print_(red, "Error - Invalid for non - iterative function[", f, "]\n");
								print_(blue, "To overwrite the definition of[", f, "] first use $clean ", f, '\n');
								throw_();
							}
							else {
								if (f_var_allnum) {
									func_def[f].partial1[f_var] = f_exp;
								}
								else {
									func_def[f].partial2[f_var] = f_exp;
								}
							}
						}
						else {//将函数存入func_def数组
							if (func_def[f].def.expr.size() > 0 && f_exp.expr != func_def[f].def.expr) {
								func_def[f] = { equ_pos / 2 - 1, f_var, f_exp };
							}
							else {
								func_def[f] = { equ_pos / 2 - 1, f_var, f_exp, func_def[f].partial1, func_def[f].partial2 };
							}
							func_ite.erase(f);
							for (auto j = func_ite.begin(); j != func_ite.end(); ++j) {
								if (has(f_exp.expr, *j)) {
									func_ite.insert(f);
									break;
								}
							}
							if (has(f_exp.expr, f)) {
								func_ite.insert(f);
							}
						}
					}
					else {
						if (f_isdef) {
							if (f_var_allnum) {
								func_def[f] = { equ_pos / 2 - 1,{},{},{ { f_var,f_exp } },{} };
							}
							else {
								func_def[f] = { equ_pos / 2 - 1,{},{},{},{ { f_var,f_exp } } };
							}
							func_ite.insert(f);
						}
						else {
							func_def[f] = { equ_pos / 2 - 1, f_var, f_exp };
							func_ite.erase(f);
							for (auto j = func_ite.begin(); j != func_ite.end(); ++j) {
								if (has(f_exp.expr, *j)) {
									func_ite.insert(f);
									break;
								}
							}
							if (has(f_exp.expr, f)) {
								func_ite.insert(f);
							}
						}
					}
					exp_re = "Function [" + f + "] Defined.";
				}
			}
			else {
				exp_re = "Error - Unexpected [=]";
			}
		}
		else {
			exp_re = toString(cal(exp_arr));
			is_exp_re = true;
		}
	}
	return exp_re;
}

void script(int arg, char* args[]) {
	set<string> option;//参数列表
	map<string,int> label;//存储标签
	map<string, int> color = { {"r",4},{"g",2},{"b",1},{"w",7},{ "ir",12 },{ "ig",10 },{ "ib",9 },{ "iw",15 } };
	vector<string> code;
	string filename(args[1]);
	string line;
	switches["notime"] = true;
	for (int i = 2; i < arg; ++i) {
		option.emplace(args[i]);
	}
	if (filename.substr(filename.size() - 3) == ".cp") {
		ifstream in(filename.c_str(), ios::in);
		if (!in) {
			print_(red, "Unable to open file ", filename,'\n');
			return;
		}
		while (!in.eof()) {
			getline(in, line);
			if (line.size() == 0) {
				continue;
			}
			auto lines = split(line, ';');
			code.insert(code.end(), lines.begin(), lines.end());
		}
		in.close();
		bool err_out = !has(option, { "/no_err" })&&!has(option, { "-no_err" });
		bool echo = !has(option, { "/no_echo" })&&!has(option, { "-no_echo" });
		for (int i = 0, j = code.size(); i < j; ++i) {
			if (code[i][0] == ':') {//如果是标签则存储
				label[code[i].substr(1)] = i;
			}
		}
		map<string, bool(*)(double)> if_statement = { {">?",[](double x) {return x > 0; }},{"=?",[](double x) {return x == 0; }},{"<?",[](double x) {return x < 0; }} };
		for (int i = 0, j = code.size(); i < j; ++i) {
			if (broken) {
				broken = false;
				Sleep(10);
				return;
			}
			line = code[i];
			++input_count;
			bool is_exp_re;
			auto cmd = split(line, ' ');
			try {
				if (cmd[0][0] == '#') {//忽略注释行
					continue;
				}
				else if (cmd[0][0] == '!') {//输出一行
					cout << line.substr(1);
				}
				else if (cmd[0][0] == '/') {//换行
					for (int k = 0; k < cmd[0].size(); ++k) {
						printf("\n");
					}
				}
				else if (cmd[0][0] == '%') {//输出变量值
					line = cmd[0].substr(1);
					if (has(var_def, line)) {
						cout << var_def[line];
					}
				}
				else if (cmd[0][0] == '@') {//改变输出颜色
					if (cmd[0].size() == 1) {
						setc(7);
					}
					else {
						if (has(color, cmd[0].substr(1))) {
							setc(color[cmd[0].substr(1)]);
						}
						else {
							setc(stoi(cmd[0].substr(1)));
						}
					}
				}
				else if (cmd[0][0] == ':') {//跳过标签
					continue;
				}
				else if (cmd[0][0] == '?') {
					print_(red, "Invalid if statement");
				}
				else if (cmd[0].substr(0,2) == "->") {//跳转到指定标签
					if (has(label, cmd[0].substr(2))) {
						i = label[cmd[0].substr(2)];
					}
				}
				else if (has(if_statement,cmd[0].substr(0, 2))) {//if语句
					auto exp_re = exec(line.substr(2), is_exp_re, true);
					if (is_exp_re) {
						if (!if_statement[cmd[0].substr(0, 2)](to_num(exp_re))) {
							while (code[++i] != "?");//跳出if语句体
						}
					}
					else {
						if (err_out) {
							print_(red, "Invalid if statement");
						}
					}
				}
				else if (cmd[0][0] == '=') {//从用户获取输入
					if (cmd[0].size() == 1) {
						system("pause>nul");
					}
					else {
						getline(cin, line);
						Sleep(10);
						if (broken) {
							broken = false;
							return;
						}
						auto exp_re = exec(cmd[0].substr(1) + "=" + line, is_exp_re, true);
						if (echo&&is_exp_re) {
							if (has(exp_re, "Error")) {
								print_(red, exp_re);
							}
							else if (has(exp_re, "Warning")) {
								print_(red + green, exp_re);
							}
							else {
								print_(white, exp_re);
							}
							printf("\n");
						}
					}
				}
				else if (cmd[0] == "$exit") {
					return;
				}
				else {
					auto exp_re=exec(line,is_exp_re,true);
					if (echo&&is_exp_re) {
						if (has(exp_re, "Error")) {
							print_(red, exp_re);
						}
						else if (has(exp_re, "Warning")) {
							print_(red + green, exp_re);
						}
						else {
							print_(white, exp_re);
						}
						printf("\n");
					}
				}
			}
			catch (err e) {
				if (err_out) {
					if (e.type == e.warning) {
						print_(green + red, e.msg, '\n');
					}
					else if (e.type == e.error) {
						print_(red, e.msg, '\n');
					}
					else if (e.type == e.tip) {
						print_(blue + green, e.msg, '\n');
					}
				}
			}
			catch (...) {
				//不处理
			}
		}
	}
	else {
		print_(red, "Invalid input.\n");
	}
}

int main(int arg,char* args[]) {
	system("title Calc Pro Plus - John Ao");
	srand(clock());
	signal(SIGINT, break_handler);
	signal(SIGBREAK, break_handler);
	if (arg > 1) {
		script(arg, args);
		return 0;
	}
	randomHead();
	print_(white, " ************************************\n *     Calc Pro Plus by John Ao     *\n *       查看帮助请输入 $help       *\n *      请注意切换至英文输入法      *\n ************************************\n");
	stream.precision(13);//设置double输出精度
	cout.precision(13);
	bool easterEgg = false;//彩蛋
	while (true) {
		string e_in;
		cout << "\nIn:=";
		try {
			getline(cin, e_in);
			Sleep(10);
			if (broken) {
				broken = false;
				throw_();
			}
			while (e_in.size() > 0 && e_in[0] == ' ') {
				e_in.assign(e_in.begin() + 1, e_in.end());
			}
			if (e_in.size() == 0)
				throw_();
		}
		catch (...) {
			continue;
		}
		vector<string> in_list = split(e_in, ';');//处理多行输入
		for (int i = 0; i < in_list.size(); ++i) {
			++input_count;//输出计数
			string exp_in = in_list[i], exp_re;
			bool is_exp_re = false;//是否有返回值
			int time_s = clock();//记录开始时间
			try {
				exp_re=exec(exp_in, is_exp_re);
			}
			catch (err e) {//按类型分颜色输出警告
				if (e.type == e.warning) {
					print_(green + red, e.msg, '\n');
				}
				else if (e.type == e.error) {
					print_(red, e.msg, '\n');
				}
				else if (e.type == e.tip) {
					print_(blue + green, e.msg, '\n');
				}
			}
			catch (...) {
				//其他类型错误不处理
			}
			if (exp_re.size() > 0) {
				print_(green, "Out[", input_count, "]=");
				if (is_exp_re) {
					func_def["out"].partial1[{toString(input_count)}] = { exp_re };
					if (switches["fraction"]) {//数学输出设置
						string exp_re_ = prettyPrint(to_num(exp_re));
						if (exp_re_ != ""&&exp_re != exp_re_) {
							print_(white, exp_re_, '\n');
							int space = int(6 + log10(input_count));
							for (int i = 0; i < space; ++i) {
								cout << ' ';
							}
							print_(green, "≈");
							print_(white, exp_re);
						}
						else {
							print_(white, exp_re);
						}
					}
					else {
						print_(white, exp_re);
					}
					if (exp_re == "2048") {
						if (easterEgg) {
							print_(blue + green, "\n使用方向键或awsd操控，按r重新开始，按Esc退出游戏\n");
							int x, y;
							mapping::getCursor(x, y);
							bool inGame = true;
							while (inGame) {
								try {
									mapping::setPos(x, y);
									initGame(x, y);
								}
								catch (char*) {
									inGame = false;
									mapping::setPos(0, y + 16);
								}
								catch (...) {
									Sleep(500);
									system("pause>nul");
								}
							}
							mapping::showCursor();
							easterEgg = false;
							switches["notime"] = true;
						}
						else {
							easterEgg = true;
							print_(blue + green, "\n再次输入2048开始游戏");
						}
					}
					else {
						easterEgg = false;
					}
				}
				else {
					if (has(exp_re, "Error")) {
						print_(red, exp_re);
					}
					else if (has(exp_re, "Warning")) {
						print_(red + green, exp_re);
					}
					else {
						print_(white, exp_re);
					}
				}
				cout << '\n';
			}
			if (switches["time"]&&!switches["notime"]) {//输出运行时间
				print_(red + green - intense, "Time used: ", (clock() - time_s) / 1e3, "s\n");
				switches["notime"] = false;
			}
			cout << '\n';
		}
	}
}