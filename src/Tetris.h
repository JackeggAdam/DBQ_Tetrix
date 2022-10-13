#pragma once
#include <vector>
#include <graphics.h>
#include "Block.h"
using namespace std;
class Tetris
{
public:
	Tetris(int rows, int cols, int left, int top, int blockSize);
	void init(); //初始化
	void play(); //开始游戏

private:
	int getDelay();
	void drop();
	void keyEvent();
	void clearLine();
	void updateWindow();
	void rotate();
	void moveLeftRight(int offset);
	void drawGameData();
	void checkOver();
	void saveScore();
	void displayOver();
private:
	int rows_;//游戏区域行数
	int cols_;//游戏区域列数
	int left_margin_;//游戏区域左边界
	int top_margin_;//游戏区域上边界
	int block_size_;//游戏方块大小
	IMAGE back_ground_image_;//游戏背景图片
	IMAGE game_win_image_;//游戏胜利图标
	IMAGE game_over_image_;//游戏结束图标
	
	int block_drop_delay_;//方块下落速度
	bool render_update_;//画面渲染标志

	int score_;//分数
	int level_;//难度等级
	int line_count_;
	int highest_score_;//历史最高分
	bool win_game_;//游戏胜利
	bool game_over_;//游戏结束

	Block* cur_block_;//当前下落的方块
	Block* next_block_;//下一个下落的方块

	vector<vector<int>> game_map_;//游戏地图
	static IMAGE s_clear_block_imgs_[12];//12帧消除特效的图片
};

