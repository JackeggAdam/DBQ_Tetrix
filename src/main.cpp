#include <graphics.h>//ͼ���
#include <conio.h>//����̨��������� 
#include "Tetris.h"
#include "Block.h"

int main()
{
	Tetris game(20, 10, 263, 133, 36);
	game.play();
	return 0;
}