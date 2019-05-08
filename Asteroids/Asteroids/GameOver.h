#ifndef GAMEOVER_H_INCLUDE
#define GAMEOVER_H_INCLUDE

#include "GameState.h"
#include <list>
#include <memory>
class Font;

class GameOver : public GameState
{
public:
	GameOver();

	void OnActivate(System *system, StateArgumentMap &args);
	void OnUpdate(System *system);
	void OnRender(System *system);
	void OnDeactivate(System *system);

private:
	int delay_;
	Font *font_;
	Font *scoreFont_;
	
	typedef std::list<std::shared_ptr<int>> HighScoreList;
	HighScoreList highscores_;
};

#endif // GAMEOVER_H_INCLUDE
