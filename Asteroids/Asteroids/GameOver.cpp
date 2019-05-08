#include "GameOver.h"
#include "System.h"
#include "Graphics.h"
#include "Font.h"
#include "Game.h"

GameOver::GameOver() :
	delay_(0),
	font_(0),
	scoreFont_(0)
{
}

void GameOver::OnActivate(System *system, StateArgumentMap &args)
{
	delay_ = 180;
	if (font_ == 0)
	{
		font_ = system->GetGraphics()->CreateXFont("Arial", 48);
	}
	if (scoreFont_ == 0)
	{
		scoreFont_ = system->GetGraphics()->CreateXFont("Arial", 28);
	}
	highscores_ = system->GetGame()->GetHighScoreList();	// This is a list of integer smart pointers. 
															// When ref count goes to zero, it will get freed automatically
}

void GameOver::OnUpdate(System *system)
{
	if (--delay_ == 0)
	{
		system->SetNextState("MainMenu");
	}
}

void GameOver::OnRender(System *system)
{
	system->GetGame()->RenderBackgroundOnly(system->GetGraphics());

	const char *gameOverText = "Game Over";
	int textWidth = font_->CalculateTextWidth(gameOverText);
	int textX = (800 - textWidth) / 2;
	int textY = (600 - 100) / 2;

	font_->DrawText(gameOverText, textX, textY - 25, 0xffffff00);

	const char *highscoreTitleText = "High Scores";

	scoreFont_->DrawText(highscoreTitleText, textX + 30, textY + 50, 0xffffff00);

	const int MAX_COUNT = 3;
	int scorePosY = 110;
	int count = 0;
		for (HighScoreList::const_iterator highscoreIt = highscores_.begin(),
	end = highscores_.end();
	highscoreIt != end &&
		count < MAX_COUNT;
	++highscoreIt)
	{
		count++;
		char highScore[256];
		sprintf_s(highScore, "%d. %d", count, **highscoreIt);
		scoreFont_->DrawText(highScore, textX + 50, textY + scorePosY, 0xffffff00);
		scorePosY += 50;
	}
	
}

void GameOver::OnDeactivate(System *system)
{
	system->GetGraphics()->DestroyXFont(font_);
	system->GetGraphics()->DestroyXFont(scoreFont_);
	scoreFont_ = 0;
	font_ = 0;
}
