#include "Block.h"
/*
	所有方块在初始位置的放置位置
	令数组中的第一个方块为旋转中心
	修改：扩展不同方块
	0  1  2  3
	4  5  6  7
	8  9  10 11
	12 13 14 15

*/
const vector<vector<int>> kBlocks = {
	{5},//.
	{5,1,6},//small L
	{1,0,2,3},//I 
	{5,1,6,10},//Z 1
	{5,2,6,9},//Z 2
	{5,1,4,6},//T
	{9,1,5,10},//L
	{10,2,6,9},//J
	{5,6,9,10},//田
	{2,1,0,6,10},//Big L
	{5,4,8,6,10},// N
	{5,1,4,6,9},// +
};
//
//const vector<vector<int>> kBlocks = {
//	{3,1,5,7},//I 
//	{4,2,5,7},//Z 1
//	{5,3,4,6},//Z 2
//	{5,3,4,7},//T
//	{3,2,5,7},//L
//	{5,3,7,6},//J
//	{2,3,4,5}//田
//};
int Block::s_size_ = 36;
IMAGE* Block::s_block_imgs_[8] = { nullptr };

/*
	获取方块类型所对应的图片
*/
IMAGE* Block::getImage(int index)
{
	return s_block_imgs_[index % 7];
}
//Block Ctor
Block::Block()
{
	//静态变量初始化：导入8种不同颜色方块的图片
	if (s_block_imgs_[0] == nullptr) {
		//导入含有8种方块的长条图
		IMAGE all_block_img;
		loadimage(&all_block_img, "res/tiles.png");

		SetWorkingImage(&all_block_img);
		for (int i = 0;i < 8;++i) {
			s_block_imgs_[i] = new IMAGE;
			getimage(s_block_imgs_[i], i*s_size_, 0, s_size_, s_size_);
		}
		SetWorkingImage();
	}

	//获取一个随机数，以获得随机方块类型
	block_type_ = rand() % kBlocks.size();
	//用单方块组成该方块
	for (int i = 0;i < kBlocks[block_type_].size();++i) {
		//MonoBlock tmp_monoBlock;
		v_block_set_.push_back(MonoBlock());
		v_block_set_[i].row = kBlocks[block_type_][i] / 4;
		v_block_set_[i].col = kBlocks[block_type_][i] % 4;
	}
	if (block_type_ == 0) {
		block_img_ = s_block_imgs_[7];
	}
	else {
		block_img_ = s_block_imgs_[block_type_ % 7];
	}
}

//Block Copy Ctor
Block & Block::operator=(const Block & rhs)
{
	//检测自我赋值
	if (this == &rhs)return *this;

	block_type_ = rhs.block_type_;
	v_block_set_ = rhs.v_block_set_;
	if (block_type_ == 0) {
		block_img_ = s_block_imgs_[7];
	}
	else {
		block_img_ = s_block_imgs_[block_type_ % 7];
	}
}

/*
	获取方块类型
*/
int Block::getType()
{
	return block_type_;
}

/*
	方块下落函数，即当前方块的所有单方块行数+1
*/
void Block::drop()
{
	for (auto&monoBlock : v_block_set_) {
		++monoBlock.row;
	}
}

void Block::moveLeftRight(int offset)
{
	for (auto &monoBlock : v_block_set_) {
		monoBlock.col += offset;
	}
}
/*
	C = [cx,cy] 为旋转中心
	P = [x,y] 为任意点
	P' 为绕C旋转90度后的新的坐标点
	令 R为旋转矩阵，R =[ 0 -1 ]
					   [ 1  0 ]
	有 P' = C + ( P - C ) * R
	即 P' = [cx + cy - y , -cx + cy + x]
*/
void Block::rotate()
{
	MonoBlock C = v_block_set_[0];
	for (auto& monoBlock : v_block_set_) {
		MonoBlock P = monoBlock;
		monoBlock.col = C.col + C.row - P.row;
		monoBlock.row = -C.col + C.row + P.col;
	}
}

/*
	方块碰撞检测
*/
bool Block::checkCollision(const vector<vector<int>>& map)
{
	int max_row = map.size();
	int max_col = map[0].size();
	for (auto& monoBlock : v_block_set_) {
		if (monoBlock.col < 0 ||
			monoBlock.col >= max_col ||
			monoBlock.row < 0 ||
			monoBlock.row >= max_row ||
			map[monoBlock.row][monoBlock.col] != -1) {
			return true;
		}
	}
	return false;
}
/*
	方块落地固定
*/
void Block::solidify(vector<vector<int>>& map)
{
	for (auto&monoBlock : v_block_set_) {
		map[monoBlock.row][monoBlock.col] = block_type_;
	}
}

/*
	方块渲染函数
*/
void Block::draw(int x_o,int y_o)
{
	for (auto monoBlock : v_block_set_) {
		int x = x_o+monoBlock.col*s_size_;
		int y = y_o+monoBlock.row*s_size_;
		putimage(x, y, block_img_);
	}
}

