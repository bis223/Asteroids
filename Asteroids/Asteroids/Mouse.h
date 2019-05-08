#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

#include "Game.h"
class Mouse
{
public:
	Mouse(Game *game);
	void OnLeftDown(int &x, int &y);
	void OnLeftUp(int x, int y);
	void OnRightDown(int x, int y);
	void OnRightUp(int x, int y);
private:
	Game* game_;	
};

#endif // MOUSE_H_INCLUDED