#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include <time.h>
#include <map>
#include <thread>
using namespace std;

enum dire { up, down, right, left };//方向
									//调色板
map<int, int> colorPalate = { { 2,15 },{ 4,14 },{ 8,13 },{ 16,12 },{ 32,11 },{ 64,10 },{ 128,143 },{ 256,142 },{ 512,139 },{ 1024,138 },{ 2048,112 },{ 4096,42 },{ 15,42 } };

namespace mapping {
	bool consoleLock = false;//控制台输出锁
	inline void setc(int col) {//改变输出颜色
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
	}
	inline void setPos(short x, short y, bool lock = true) {//移动光标位置
		while (lock&&consoleLock) Sleep(1);//若锁定则等待
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { x, y });
	}
	inline void hideCursor() {//隐藏光标
		auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(handle, &cursorInfo);
		cursorInfo.bVisible = false;
		SetConsoleCursorInfo(handle, &cursorInfo);
	}
	inline void showCursor() {//显示光标
		auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(handle, &cursorInfo);
		cursorInfo.bVisible = true;
		SetConsoleCursorInfo(handle, &cursorInfo);
	}
	inline void getCursor(int& x, int& y) {//获取光标位置
		CONSOLE_SCREEN_BUFFER_INFO pos;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &pos);
		x = pos.dwCursorPosition.X;
		y = pos.dwCursorPosition.Y;
	}
	map<int, char*> symbols;//mapping字符映射表
	int length = 2;//字符宽度为2
	int Ox = 0;//坐标原点
	int Oy = 0;
	class _map {
	public:
		int width;//宽、高
		int height;
		_map() :width(0), height(0) {}
		_map(int _width, int _height) :width(_width), height(_height) {
			data.resize(width*height);
		}
		_map(const _map&m) :width(m.width), height(m.height), data(m.data) {}
		int& operator()(int x, int y) {
			return data[x + y * width];
		}
		_map& operator=(const _map&m) {
			width = m.width;
			height = m.height;
			data = m.data;
			return *this;
		}
		void print(int x0 = Ox, int y0 = Oy) {//输出mapping
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					if (data[i*width + j] > 0) {
						setPos((j + x0)*length, i + y0);
						if (colorPalate.find(data[i*width + j]) != colorPalate.end()) setc(colorPalate[data[i*width + j]]);
						cout << symbols[data[i*width + j]];
						setc(7);
					}
				}
			}
		}
		void print(const _map &m, bool strict = true, int x0 = Ox, int y0 = Oy) {//非全局更新
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					if (data[i*width + j] != m.data[i*width + j] && (strict || m.data[i*width + j] != 0)) {
						if (m.data[i*width + j] >= 0) {
							setPos((j + x0)*length, i + y0);
							cout << symbols[1];
							setPos((j + x0)*length, i + y0);
							if (colorPalate.find(m.data[i*width + j]) != colorPalate.end()) setc(colorPalate[m.data[i*width + j]]);
							cout << symbols[m.data[i*width + j]];
							setc(7);
						}
					}
				}
			}
		}
		bool sizeEqu(const _map&m) {
			return width == m.width&&height == m.height;
		}
		_map merge(_map &b) {//合并
			_map &a = *this;
			if (!(a.sizeEqu(b))) {
				throw(false);
			}
			_map c = a;
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					if ((a(j, i) | b(j, i)) == 0) {
						c(j, i) = 0;
					}
					else if (a(j, i)*b(j, i) == 0) {
						c(j, i) = a(j, i) | b(j, i);
					}
					else {
						throw(false);
					}
				}
			}
			return c;
		}
		_map operator+(_map &b) {
			if (!(sizeEqu(b))) {
				throw(false);
			}
			_map &a = *this;
			_map c = *this;
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					c(j, i) = a(j, i) == 0 ? b(j, i) : a(j, i);
				}
			}
			return c;
		}
		bool collapse(_map&b) {//冲突检测
			if (!(sizeEqu(b))) {
				throw(false);
			}
			_map &a = *this;
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					if (a(j, i) != 0 && b(j, i) != 0) {
						return true;
					}
				}
			}
			return false;
		}
	private:
		vector<int> data;//存放数据
	};
};


mapping::_map toBoard(int data_[][4]) {//将4*4数组转为_map
	mapping::_map re(16, 12);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (data_[i][j] == 0) {
				continue;
			}
			re(j * 4, i * 3) = 9;
			re(j * 4 + 3, i * 3) = 10;
			re(j * 4, i * 3 + 2) = 11;
			re(j * 4 + 3, i * 3 + 2) = 12;
			re(j * 4 + 1, i * 3) = re(j * 4 + 2, i * 3) = re(j * 4 + 1, i * 3 + 2) = re(j * 4 + 2, i * 3 + 2) = 13;
			re(j * 4, i * 3 + 1) = re(j * 4 + 3, i * 3 + 1) = 14;
			re(j * 4 + 2, i * 3 + 1) = 0;
			if (mapping::symbols.find(data_[i][j]) != mapping::symbols.end()) {
				re(j * 4 + 1, i * 3 + 1) = data_[i][j];
				re(j * 4 + 2, i * 3 + 1) = -1;
			}
			else {
				re(j * 4 + 1, i * 3 + 1) = data_[i][j] = 15;
			}
		}
	}
	return re;
}

int moveBoard(int data_[][4], dire d) {//移动
	int re = 0;
	switch (d) {
	case dire::up:
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (data_[j][i] == 0) {
					for (int k = j + 1; k < 4; ++k) {
						if (data_[k][i] != 0) {
							data_[j][i] = data_[k][i];
							data_[k][i] = 0;
							break;
						}
					}
				}
			}
			for (int j = 0; j < 3; ++j) {
				if (data_[j][i] == data_[j + 1][i]) {
					re += data_[j][i] *= 2;
					for (int k = j + 1; k < 3; ++k) {
						data_[k][i] = data_[k + 1][i];
					}
					data_[3][i] = 0;
				}
			}
		}
		break;
	case dire::down:
		for (int i = 0; i < 4; ++i) {
			for (int j = 3; j > 0; --j) {
				if (data_[j][i] == 0) {
					for (int k = j - 1; k >= 0; --k) {
						if (data_[k][i] != 0) {
							data_[j][i] = data_[k][i];
							data_[k][i] = 0;
							break;
						}
					}
				}
			}
			for (int j = 3; j > 0; --j) {
				if (data_[j][i] == data_[j - 1][i]) {
					re += data_[j][i] *= 2;
					for (int k = j - 1; k > 0; --k) {
						data_[k][i] = data_[k - 1][i];
					}
					data_[0][i] = 0;
				}
			}
		}
		break;
	case dire::left:
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (data_[i][j] == 0) {
					for (int k = j + 1; k < 4; ++k) {
						if (data_[i][k] != 0) {
							data_[i][j] = data_[i][k];
							data_[i][k] = 0;
							break;
						}
					}
				}
			}
			for (int j = 0; j < 3; ++j) {
				if (data_[i][j] == data_[i][j + 1]) {
					re += data_[i][j] *= 2;
					for (int k = j + 1; k < 3; ++k) {
						data_[i][k] = data_[i][k + 1];
					}
					data_[i][3] = 0;
				}
			}
		}
		break;
	case dire::right:
		for (int i = 0; i < 4; ++i) {
			for (int j = 3; j > 0; --j) {
				if (data_[i][j] == 0) {
					for (int k = j - 1; k >= 0; --k) {
						if (data_[i][k] != 0) {
							data_[i][j] = data_[i][k];
							data_[i][k] = 0;
							break;
						}
					}
				}
			}
			for (int j = 3; j > 0; --j) {
				if (data_[i][j] == data_[i][j - 1]) {
					re += data_[i][j] *= 2;
					for (int k = j - 1; k > 0; --k) {
						data_[i][k] = data_[i][k - 1];
					}
					data_[i][0] = 0;
				}
			}
		}
		break;
	}
	return re;
}

void loseUI(int data_[][4]) {//失败输出
	int cls[4][4] = { 0 };
	toBoard(data_).print(toBoard(cls));
	mapping::setPos(mapping::Ox + 13, mapping::Oy + 5);
	mapping::setc(12);
	cout << "YOU LOSE!";
	mapping::setc(7);
	system("pause>nul");
	throw(false);
}

void newPiece(int data_[][4]) {//随机放新方块
	vector<int> available;
	for (int i = 0; i < 16; ++i) {
		if (data_[i / 4][i % 4] == 0) {
			available.push_back(i);
		}
	}
	if (available.size() == 0) {
		loseUI(data_);
		throw(false);
	}
	int p = available[rand() % available.size()];
	data_[p / 4][p % 4] = rand() % 6 ? 2 : 4;
}

bool isLose(int data_[][4]) {//判负
	int data_backup[4][4];
	bool re = false;
	memcpy(data_backup, data_, 16 * sizeof(int));
	moveBoard(data_backup, dire::up);
	re |= (memcmp(data_, data_backup, 16 * sizeof(int)) != 0);
	memcpy(data_backup, data_, 16 * sizeof(int));
	moveBoard(data_backup, dire::down);
	re |= (memcmp(data_, data_backup, 16 * sizeof(int)) != 0);
	memcpy(data_backup, data_, 16 * sizeof(int));
	moveBoard(data_backup, dire::right);
	re |= (memcmp(data_, data_backup, 16 * sizeof(int)) != 0);
	memcpy(data_backup, data_, 16 * sizeof(int));
	moveBoard(data_backup, dire::left);
	re |= (memcmp(data_, data_backup, 16 * sizeof(int)) != 0);
	return !re;
}

void _timer(int startTime, bool** TimerRun) {//计时器
	bool run = true;
	*TimerRun = &run;//销毁标记
	while (run) {
		mapping::consoleLock = true;
		mapping::setPos(mapping::Ox + 37, mapping::Oy + 3, false);
		cout << (clock() - startTime) / 1e3 << "s        ";
		mapping::consoleLock = false;
		Sleep(80);
	}
}

void initGame(int x, int y) {
	int score = 0;//计分器
	mapping::Ox = x + 1;//坐标原点
	mapping::Oy = y + 1;
	mapping::symbols = { { 0,"" },{ 1,"　" },{ 2," ２ " },{ 4," ４ " },{ 8," ８ " },{ 9,"┏─" },{ 10,"┓" },{ 11,"┗─" },{ 12,"┛" },{ 13,"──" },{ 14,"│" },{ 15," ∞ " },{ 16," 16 " },{ 32," 32 " },{ 64," 64 " },{ 128,"１28" },{ 256,"２56" },{ 512,"５12" },{ 1024,"1024" },{ 2048,"2048" },{ 4096,"4096" } };
	mapping::hideCursor();
	int data_[4][4] = {};
	srand(clock());//初始化随机数
				   //打印盘面
	cout << "╔";
	for (int i = 0; i < 17; ++i) {
		cout << "══";
	}
	cout << "╗┏═════──────\n";
	for (int i = 0; i < 12; ++i) {
		cout << "║";
		for (int j = 0; j < 17; ++j) {
			cout << "　";
		}
		cout << "║│\n";
	}
	cout << "╚";
	for (int i = 0; i < 17; ++i) {
		cout << "══";
	}
	cout << "╝\n";
	mapping::setPos(x + 38, y + 1);
	mapping::setc(13);
	cout << "Score";
	mapping::setPos(x + 38, y + 2);
	cout << 0 << "             ";   
	mapping::setPos(x + 38, y + 3);
	mapping::setc(10);
	cout << "Time";
	mapping::setPos(x + 38, y + 4);
	cout << 0 << "             ";
	mapping::setc(7);
	newPiece(data_);
	newPiece(data_);
	mapping::_map currentBoard = toBoard(data_);//当前局面
	currentBoard.print();
	int data_backup[4][4];
	bool firstRun = true;//第一次运行
	bool timing = false;//计时中
	thread Timer;//计时器线程
	bool *TimerRun = NULL;//计时器销毁标记
	char ch;
	bool moved;
	while (true) {
		mapping::setPos(x + 38, y + 2);
		cout << score << "        ";//输出分数
		if (isLose(data_)) {
			if (timing) {
				*TimerRun = false;
				timing = false;
			}
			Sleep(300);
			loseUI(data_);
		}
		ch = _getch();
		if (ch == -32) ch = _getch();
		mapping::_map tmp = currentBoard;
		moved = false;
		memcpy(data_backup, data_, 16 * sizeof(int));
		if (firstRun) {//从第一个按键开始计时
			firstRun = false;
			Timer = thread(_timer, clock(), &TimerRun);
			Timer.detach();
			timing = true;
		}
		switch (ch) {
		case 72:
		case 'w':
			score += moveBoard(data_, dire::up);
			moved = memcmp(data_backup, data_, 16 * sizeof(int));
			break;
		case 75:
		case 'a':
			score += moveBoard(data_, dire::left);
			moved = memcmp(data_backup, data_, 16 * sizeof(int));
			break;
		case 77:
		case 'd':
			score += moveBoard(data_, dire::right);
			moved = memcmp(data_backup, data_, 16 * sizeof(int));
			break;
		case 80:
		case 's':
			score += moveBoard(data_, dire::down);
			moved = memcmp(data_backup, data_, 16 * sizeof(int));
			break;
		case 'r':
			for (int i = 0; i < 16; ++i) {
				data_[i / 4][i % 4] = 0;
			}
			score = 0;
			if (timing) {
				*TimerRun = false;
				timing = false;
				mapping::setPos(mapping::Ox + 37, mapping::Oy + 3);
				cout << "0            ";
			}
			tmp = toBoard(data_);
			currentBoard.print(tmp);
			newPiece(data_);
			newPiece(data_);
			currentBoard = toBoard(data_);
			currentBoard.print();
			firstRun = true;
			break;
		case 27:
			if (timing) {
				*TimerRun = false;
				timing = false;
			}
			throw("Exit");
			break;
		}
		if (moved) {//更新局面
			tmp = toBoard(data_);
			currentBoard.print(tmp);
			currentBoard = tmp;
			Sleep(200);
			newPiece(data_);
			tmp = toBoard(data_);
			currentBoard.print(tmp);
			currentBoard = tmp;
		}
		currentBoard.print();
		while (_kbhit()) _getch();
	}
}