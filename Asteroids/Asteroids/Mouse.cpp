#include "Mouse.h"
#include "System.h"

Mouse::Mouse(Game *game): game_(game)
{

}

void Mouse::OnLeftDown(int &x, int &y)
{
	game_->OnLeftDown(x, y);
}

void Mouse::OnLeftUp(int x, int y)
{
	game_->OnLeftUp(x, y);
}

void Mouse::OnRightDown(int x, int y)
{
	game_->OnRightDown(x, y);
}

void Mouse::OnRightUp(int x, int y)
{
	game_->OnRightUp(x, y);
}
