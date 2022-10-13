#pragma once
#include <graphics.h>
#include <vector>
using namespace std;

struct MonoBlock {
	int row;
	int col;
	MonoBlock():row(0),col(0){}
};

class Block
{


public:
	static IMAGE* getImage(int index);


public:
	Block();//ctor
	Block& operator=(const Block& rhs);//copy ctor

	int getType();
	void drop();
	void moveLeftRight(int offset);
	void rotate();
	bool checkCollision(const vector<vector<int>>&map);
	void solidify(vector<vector<int>>& map);
	void draw(int x_o,int y_o);

private:
	
	static IMAGE* s_block_imgs_[8];//7����ɫ�����ͼƬ
	static int s_size_;//����Ĵ�С

	int block_type_;//��������
	//�÷�������MonoBlock������
	vector<MonoBlock> v_block_set_;
	IMAGE *block_img_;//������ɫ
};

