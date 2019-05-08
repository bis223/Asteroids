#include "PlayingState.h"
#include "System.h"
#include "Game.h"

PlayingState::PlayingState()
{
}

PlayingState::~PlayingState()
{
}

void PlayingState::OnActivate(System *system, StateArgumentMap &args)
{
	Game *game = system->GetGame();
	level_ = args["Level"].asInt;
	game->InitialiseLevel(level_);
}

void PlayingState::OnUpdate(System *system)
{
	Game *game = system->GetGame();
	game->Update(system);

	if (game->IsGameOver())  // BUG FIXED: Condition to check gameover has to be done first.
	{
		system->SetNextState("GameOver");
		game->GameOver();		
	}
	else if (game->IsLevelComplete())
	{
		StateArgumentMap args;
		args["Level"].asInt = level_ + 1;
		system->SetNextState("LevelStart", args);
	}
}

void PlayingState::OnRender(System *system)
{
	Game *game = system->GetGame();
	game->RenderEverything(system->GetGraphics());
}

void PlayingState::OnDeactivate(System *system)
{
}
