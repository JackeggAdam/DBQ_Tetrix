#include <graphics.h>//图像库
#include <conio.h>//控制台输入输出库 
#include "Tetris.h"
#include "Block.h"

int main()
{
	Tetris game(20, 10, 263, 133, 36);
	game.play();
	return 0;
}