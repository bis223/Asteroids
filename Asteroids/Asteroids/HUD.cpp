#include "HUD.h"
#include "System.h"
#include "Graphics.h"
#include "Font.h"
#include "Game.h"
#include <time.h>

HUD::HUD() : lives_(3), score_(0), highScore_(0), font_(0)
{
	scoreText_.x_ = 10;
	scoreText_.y_ = 10;
	livesText_.x_ = 120;
	livesText_.y_ = 10;
	highScoreText_.x_ = 220;
	highScoreText_.y_ = 10;

	
	sprintf_s(scoreText_.text_, "Score %d", score_);
	sprintf_s(livesText_.text_, "Lives %d", lives_);
}

HUD::~HUD()
{
	if(font_)
		graphics_->DestroyXFont(font_);
}


void HUD::Update(System *system)
{
	if (font_ == 0)
	{
		font_ = system->GetGraphics()->CreateXFont("Arial", 20);
		graphics_ = system->GetGraphics();
	}

	std::vector<UIText>::iterator floatingScoreIt  = floatingScoreTexts_.begin();
	
	while(floatingScoreIt != floatingScoreTexts_.end())
	{
		if(clock() > (*floatingScoreIt).timer_)
		{	
			floatingScoreIt = floatingScoreTexts_.erase(floatingScoreIt); // erasing timed out text
		}
		else
		{
			// moving text upwards
			D3DXVECTOR3 newPosition = D3DXVECTOR3(static_cast<float>((*floatingScoreIt).x_), static_cast<float>((*floatingScoreIt).y_), 100.0f);
			D3DXVec3Add(&newPosition, &newPosition, &D3DXVECTOR3(0.0f, -1.0f, 0.0f));
			(*floatingScoreIt).x_ = static_cast<int>(newPosition.x);
			(*floatingScoreIt).y_ = static_cast<int>(newPosition.y);
			floatingScoreIt++;
		}
 	}

}

void HUD::Render(Graphics *graphics) const
{
	font_->DrawText(scoreText_.text_, scoreText_.x_, scoreText_.y_, 0xffffff00);
	font_->DrawText(highScoreText_.text_, highScoreText_.x_, highScoreText_.y_, 0xffffff00);
	font_->DrawText(livesText_.text_, livesText_.x_, livesText_.y_, 0xffffff00);

	const char * info = "Left click to move. Right click to fire.\nTap 'B' to switch firing style.";
	font_->DrawText(info, 10, 548, 0xffffff00);


	for (std::vector<UIText>::const_iterator floatingScoreIt = floatingScoreTexts_.begin(),
	end = floatingScoreTexts_.end();
	floatingScoreIt != end;
	++floatingScoreIt)
	{
		font_->DrawText((*floatingScoreIt).text_, (*floatingScoreIt).x_, (*floatingScoreIt).y_, 0xffffff00);		
	}
}

void HUD::SetLives(int lives)
{
	lives_ = lives;
	sprintf_s(livesText_.text_, "Lives %d", lives_);
}

void HUD::SetScore(int newScore)
{
	score_ += newScore;
	sprintf_s(scoreText_.text_, "Score %d", score_);
}

void HUD::SetHighScore(int newScore)
{
	highScore_ = newScore;
	sprintf_s(highScoreText_.text_, "HighScore %d", highScore_);
}

void HUD::Reset()
{
	const int TOTAL_LIVES = 3;
	score_ = 0;
	lives_ = TOTAL_LIVES;
	sprintf_s(scoreText_.text_, "Score %d", score_);
	sprintf_s(livesText_.text_, "Lives %d", lives_);
	
	floatingScoreTexts_.clear();
}

void HUD::ShowFloatingScore(int score, int x, int y)
{	
	const long FLOATING_SCORE_LIFE_TIME = 2000;
	UIText floatingScoreText;
	sprintf_s(floatingScoreText.text_, "+%d", score);
	floatingScoreText.x_ = x;
	floatingScoreText.y_ = y;
	floatingScoreText.timer_ = clock() + FLOATING_SCORE_LIFE_TIME;
	floatingScoreTexts_.push_back(floatingScoreText);
}