#include "Tetris.h"
#include "Block.h"
#include <stdlib.h>
#include <time.h>
#include<Windows.h>
#include <conio.h>
#include <fstream>
#include <mmsystem.h>
#include <string>
#pragma comment(lib, "winmm.lib")

const int kMaxLevel = 5;
const string kRecordFile = "recorder.txt";

const int kSpeedArray[6] = { 0,500,350,250,180,150 };
const int kSpeedQuick = 50;
IMAGE Tetris::s_clear_block_imgs_[12];

Tetris::Tetris(int rows, int cols, int left, int top, int blockSize):
	rows_(rows),
	cols_(cols),
	left_margin_(left),
	top_margin_(top),
	block_size_(blockSize),
	score_(0),
	level_(1),
	line_count_(0),
	game_over_(false)
{
	// 随机种子
	srand(time(nullptr));

	// 游戏窗口
	initgraph(938, 896);

	// 加载背景
	loadimage(&back_ground_image_, "../res/bg2.png");
	loadimage(&game_win_image_, "../res/win.png");
	loadimage(&game_over_image_, "../res/over.png");

	//静态变量初始化：导入12帧消除特效的图片
	string file_path = "../res/tx/";
	string png_suffix = ".png";
	for (int i = 0;i < 11;++i) {
		string tmp = file_path + to_string(i) + png_suffix;
		const char * picture_path = (tmp).c_str();
		IMAGE clear_line_png;
		loadimage(&clear_line_png, picture_path);
		SetWorkingImage(&clear_line_png);
		getimage(&s_clear_block_imgs_[i], 10, 60, block_size_*cols_, blockSize);
		SetWorkingImage();
	}
	

	// 初始化地图
	game_map_.resize(rows, vector<int>(cols, -1));
}

void Tetris::init()
{
	mciSendString("play ../res/bg.mp3 repeat", 0, 0, 0);

	render_update_ = true;
	score_ = 0;
	level_ = 1;
	block_drop_delay_ = kSpeedArray[level_];
	line_count_ = 0;
	game_over_ = false;
	win_game_ = false;
	//载入历史最高分数
	ifstream file(kRecordFile);
	if (!file.is_open()) {
		highest_score_ = 0;
	}
	else {
		file >> highest_score_;
	}
	file.close();
	//重置游戏地图
	for (int i = 0;i < rows_;++i) {
		for (int j = 0;j < cols_;++j) {
			game_map_[i][j] = -1;
		}
	}
	//初始化当前方块与Next方块
	cur_block_ = new Block();
	next_block_ = new Block();
}

void Tetris::play()
{
	//游戏初始化
	init();

	//负责方块自动下落的画面渲染计时器
	int timer = 0;
	while (1) {

		keyEvent();//检测键盘按下
		
		timer += getDelay();//更新计时器
		/*
			方块自动下落
		*/
		if (timer > block_drop_delay_) {
			timer = 0;
			render_update_ = true;
			drop();
		}

		/*
			当旋转，平移方块时，需要马上渲染画面，不然看起来会很卡
		*/
		if (render_update_) {
			render_update_ = false;
			clearLine();
			updateWindow();
		}
		/*
			处理游戏结束
		*/
		if (game_over_) {
			saveScore();//保存最高分数
			displayOver();//显示游戏结束的图片
			system("pause");
			init();
		}

	}
	
	
}
/*
	获得最近一次执行getDelay()函数在多少us之前。
*/
int Tetris::getDelay()
{
	static unsigned int s_lastTime = 0;
	unsigned int current_time = GetTickCount();
	
	if (s_lastTime == 0) {
		s_lastTime = current_time;
		return 0;
	}

	int ret = current_time - s_lastTime;
	s_lastTime = current_time;
	return ret;
}

/*
	游戏下落操作
*/
void Tetris::drop()
{
	Block bak_block = *cur_block_;
	cur_block_->drop();

	if (cur_block_->checkCollision(game_map_)) {
		bak_block.solidify(game_map_);
		delete cur_block_;

		cur_block_ = next_block_;
		next_block_ = new Block();
		block_drop_delay_ = kSpeedArray[level_];

		checkOver();
	}
}

void Tetris::keyEvent()
{
	int offset = 0;//left :-1 right:1 dontswift:0
	bool rotateFlag = false;
	if (_kbhit()) {
		unsigned char ch = _getch();
		if (ch == 224) {
			ch = _getch();
			switch (ch)
			{
			case 72: rotateFlag = true;break;
			case 80: block_drop_delay_ = kSpeedQuick;break;
			case 75: offset = -1;break;
			case 77: offset = 1;break;
			default:
				break;
			}
		}
	}
	/*
		处理旋转
	*/
	if (rotateFlag) {
		rotate();
		render_update_ = true;
	}
	/*
		处理左右移动
	*/
	if (offset != 0) {
		moveLeftRight(offset);
		render_update_ = true;
	}
}

void Tetris::clearLine()
{
	int target_line = rows_-1;
	int clear_line_num = 0;
	vector<int> clear_target_rows;
	/*
		从下往上进行重新赋值，如果处于下面的一行被占满
		则用上面的那一行对其进行覆盖，从而起到消除一行的效果
	*/
	for (int i = rows_ - 1;i >= 0;--i) {
		bool exist_holo = false;
		bool exist_lucky_block = false;
		for (int j = 0;j < cols_;++j) {
			if (game_map_[i][j] == -1) {
				exist_holo = true;
			}
			if (game_map_[i][j] == 0) {
				exist_lucky_block = true;
			}
			//将遍历到的行覆盖到目标行
			game_map_[target_line][j] = game_map_[i][j];
		}
		if (exist_holo&&!exist_lucky_block) {
			--target_line;
		}
		else {
			clear_target_rows.push_back(i);
			++clear_line_num;
		}
	}
	if (clear_line_num != 0) {
		mciSendString("play xiaochu2.mp3", 0, 0, 0);
		
		for (int i = 0;i < 11;++i) {
			for (int tar_line : clear_target_rows) {
				putimage(left_margin_, top_margin_+ tar_line*block_size_, &s_clear_block_imgs_[i]);
			}
			Sleep(50);
		}
		render_update_ = true;
		int cur_score = 0;
		score_ += clear_line_num * 10;
		if (score_ > 700) {
			game_over_ = true;
			win_game_ = true;
		}

		level_ = min(score_ / 100 + 1, kMaxLevel);
		line_count_ += clear_line_num;
		block_drop_delay_ = kSpeedArray[level_];
	}

}

void Tetris::updateWindow()
{
	//放置背景图片
	putimage(0, 0, &back_ground_image_);

	//将图片写入缓冲
	BeginBatchDraw();

	//渲染已固化的方块图片
	for (int i = 0;i < rows_;++i) {
		for (int j = 0;j < cols_;++j) {
			if (game_map_[i][j] == -1)continue;
			int pt_x = j * block_size_ + left_margin_;
			int pt_y = i * block_size_ + top_margin_;
			putimage(pt_x, pt_y, Block::getImage(game_map_[i][j]));
		}
	}

	//渲染当前下落中的方块
	cur_block_->draw(left_margin_,top_margin_);
	//渲染下一个方块
	next_block_->draw(left_margin_+(cols_+2)*block_size_,top_margin_);
	//渲染得分数据
	drawGameData();

	//对缓冲中的图片进行渲染
	EndBatchDraw();
}

void Tetris::rotate()
{
	//todo 中心对称方块不进行旋转
	Block bak_block = *cur_block_;
	cur_block_->rotate();

	if (cur_block_->checkCollision(game_map_)) {
		*cur_block_ = bak_block;
	}
}

void Tetris::moveLeftRight(int offset)
{
	Block bak_block = *cur_block_;
	cur_block_->moveLeftRight(offset);

	if (cur_block_->checkCollision(game_map_)) {
		*cur_block_ = bak_block;
	}
}

void Tetris::drawGameData()
{
	setcolor(RGB(183, 183, 183));
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 60;
	f.lfWeight = 30;
	f.lfQuality = ANTIALIASED_QUALITY;
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Segoe UI Black"));
	gettextstyle(&f);

	setbkmode(TRANSPARENT);

	string dataText;

	dataText = to_string(score_);
	outtextxy(670, 750, dataText.c_str());

	dataText = to_string(level_);
	outtextxy(195 - f.lfWeight,755, dataText.c_str());

	dataText = to_string(line_count_);
	int xPos = 195 - f.lfWeight*dataText.size();
	outtextxy(xPos, 835, dataText.c_str());

	dataText = to_string(highest_score_);
	outtextxy(670, 835, dataText.c_str());
}

void Tetris::checkOver()
{
	game_over_ = (cur_block_->checkCollision(game_map_) == true);
	win_game_ = false;
}

void Tetris::saveScore()
{
	if (score_ > highest_score_) {
		highest_score_ = score_;

		ofstream file(kRecordFile);
		file << highest_score_;
		file.close();
	}
}

void Tetris::displayOver()
{
	mciSendString("stop ../res/bg.mp3", 0, 0, 0);
	if (win_game_) {
		putimage(260, 360, &game_win_image_);
		mciSendString("play ../res/win.mp3", 0, 0, 0);
	}
	else {
		putimage(260, 360, &game_over_image_);
		mciSendString("play ../res/over.mp3", 0, 0, 0);
	}
}
