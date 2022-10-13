#pragma once
#include <vector>
#include <graphics.h>
#include "Block.h"
using namespace std;
class Tetris
{
public:
	Tetris(int rows, int cols, int left, int top, int blockSize);
	void init(); //��ʼ��
	void play(); //��ʼ��Ϸ

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
	int rows_;//��Ϸ��������
	int cols_;//��Ϸ��������
	int left_margin_;//��Ϸ������߽�
	int top_margin_;//��Ϸ�����ϱ߽�
	int block_size_;//��Ϸ�����С
	IMAGE back_ground_image_;//��Ϸ����ͼƬ
	IMAGE game_win_image_;//��Ϸʤ��ͼ��
	IMAGE game_over_image_;//��Ϸ����ͼ��
	
	int block_drop_delay_;//���������ٶ�
	bool render_update_;//������Ⱦ��־

	int score_;//����
	int level_;//�Ѷȵȼ�
	int line_count_;
	int highest_score_;//��ʷ��߷�
	bool win_game_;//��Ϸʤ��
	bool game_over_;//��Ϸ����

	Block* cur_block_;//��ǰ����ķ���
	Block* next_block_;//��һ������ķ���

	vector<vector<int>> game_map_;//��Ϸ��ͼ
	static IMAGE s_clear_block_imgs_[12];//12֡������Ч��ͼƬ
};

