#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

#include "GameEntity.h"
#include <vector>

class Font;

class HUD : public GameEntity
{
public:
	HUD();
	~HUD();
	void Update(System *system);
	void Render(Graphics *graphics) const;

	void SetLives(int lives);	
	void SetScore(int newScore);	
	void SetHighScore(int newScore);

	int  GetLives() const { return lives_; }
	int  GetScore() const { return score_; }
	int  GetHighScore() const { return highScore_; }

	void Reset();

	void ShowFloatingScore(int score, int x, int y);

private:
	int score_;
	int highScore_;
	int lives_;	
	Font *font_;
	Graphics *graphics_;

	struct UIText
	{
		char text_[256];
		int x_,y_;
		long timer_;
	};

	UIText scoreText_;
	UIText highScoreText_;
	UIText livesText_;
	std::vector<UIText> floatingScoreTexts_;
};

#endif // HUD_H_INCLUDED