#include "Game.h"
#include "System.h"
#include "OrthoCamera.h"
#include "Background.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Explosion.h"
#include "Keyboard.h"
#include "Random.h"
#include "Maths.h"
#include "Bullet.h"
#include "Collision.h"
#include "QuadTree.h"
#include "HUD.h"
#include "MysteryShip.h"
#include "Exhaust.h"
#include <algorithm>
#include <time.h>
#include <fstream>
#include <string>
#include <memory>

#define HIGHSCORE_FILE "Resources/highscores.txt"
#define GREEN D3DCOLOR_ARGB(255, 000, 255, 000)
#define BLUE  D3DCOLOR_ARGB(255, 000, 000, 255)
#define RED   D3DCOLOR_ARGB(255, 255, 000, 000)
#define ORANGE D3DCOLOR_ARGB(255, 255, 125, 000)


Game::Game() :
	camera_(0),
	background_(0),
	player_(0),
	mysteryShip_(0),
	collision_(0),
	nextFire_(0),
	rateOfFire_(100)
{
	screenSize_.width = 800.0f;
	screenSize_.height = 600.0f;
	srand((unsigned int)time(0)); // BUG FIXED: setting seed for rand() function;
	camera_ = new OrthoCamera();
	camera_->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	camera_->SetFrustum(screenSize_.width, screenSize_.height, -100.0f, 100.0f);
	background_ = new Background(screenSize_.width, screenSize_.height);
	hud_ = new HUD();
	collision_ = new Collision(this);

	std::ifstream readHighScore;
	readHighScore.open(HIGHSCORE_FILE);
	if(readHighScore.fail())
	{
		exit(1);
	}
	int score;
	while (!readHighScore.eof())
	{		
		readHighScore >> score;
	}
	hud_->SetHighScore(score);
	readHighScore.close();
}

Game::~Game()
{
	delete camera_;
	delete background_;
	delete player_;
	delete mysteryShip_;
	DeleteAllBullets();
	DeleteAllAsteroids();
	DeleteAllExplosions();
	DeleteAllExhausts();
	delete collision_;
	delete hud_;
}

void Game::Update(System *system)
{
	UpdatePlayer(system);
	UpdateExhausts(system);
	UpdateAsteroids(system);
	UpdateMysteryShip(system);
	UpdateBullet(system);
	UpdateExplosions(system);
	UpdateCollisions();
	UpdateHUD(system);
}

void Game::RenderBackgroundOnly(Graphics *graphics)
{
	camera_->SetAsView(graphics);
	background_->Render(graphics);
}

void Game::RenderEverything(Graphics *graphics)
{
	
	camera_->SetAsView(graphics);
	
	background_->Render(graphics);
	
	if (player_)
	{
		player_->Render(graphics);

	}

	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
		++asteroidIt)
	{
		(*asteroidIt)->Render(graphics);
	}


	// BUG FIXED: bullet_ variable was uninitialised.
/*
	if (bullet_) //**TODO: Candidate for crash
	{
		bullet_->Render(graphics);
	}
*/
		//List of active bullets are rendered.
		for (BulletList::const_iterator bulletIt = bullets_.begin(),
		end = bullets_.end();
		bulletIt != end;
		++bulletIt)
		{
			if((*bulletIt)->GetIsActive())
				(*bulletIt)->Render(graphics);
		}
		

	for (ExplosionList::const_iterator explosionIt = explosions_.begin(),
		end = explosions_.end();
		explosionIt != end;
		++explosionIt)
	{
		(*explosionIt)->Render(graphics);
	}

	for (ExhaustList::const_iterator exhaustIt = exhausts_.begin(),
		end = exhausts_.end();
		exhaustIt != end;
		++exhaustIt)
	{
		(*exhaustIt)->Render(graphics);
	}


		
	if (hud_)
	{
		hud_->Render(graphics);
	}

	if(mysteryShip_ &&  clock() > mysteryShip_->GetSpawnTime())
	{
		mysteryShip_->Render(graphics);
	}
}

void Game::InitialiseLevel(int level)
{
	DeleteAllBullets();
	DeleteAllAsteroids();
	DeleteAllExplosions();
	DeleteAllExhausts();

	SpawnPlayer();
	SpawnAsteroids(level);
	if(level >= 3)
	{
		SpawnMysteryShip();
	}
}

bool Game::IsLevelComplete() const
{
	return (asteroids_.empty() && explosions_.empty());
}

bool Game::IsGameOver() const
{
	return (hud_->GetLives() <= 0 && explosions_.empty());
}

void Game::GameOver()
{
	if(hud_->GetScore() > hud_->GetHighScore())
	{
		std::ofstream writeHighScore;
		writeHighScore.open(HIGHSCORE_FILE, std::fstream::app);

		writeHighScore << std::endl;
		writeHighScore << hud_->GetScore();
		writeHighScore.close();
	}
	hud_->Reset();
	if(mysteryShip_)
	{
		DeleteMysteryShip();
	}
}


std::list<std::shared_ptr<int>> Game::GetHighScoreList()
{
	std::list<std::shared_ptr<int>> highscores;
	
	std::ifstream readHighScore;
	readHighScore.open(HIGHSCORE_FILE);
	
	int score = 0;
	while (!readHighScore.eof())
	{
		readHighScore >> score;
		highscores.push_back(std::make_shared<int>(score)); //creating a smart pointer.
	}
	readHighScore.close();
	highscores.reverse();
	return highscores;
}


void Game::DoCollision(GameEntity *a, GameEntity *b)
{
	Ship *player = static_cast<Ship *>(a == player_ ? a : (b == player_ ? b : 0));
	Bullet *bullet = static_cast<Bullet *>(IsBullet(a) ? a : (IsBullet(b) ? b : 0));
	Asteroid *asteroid = static_cast<Asteroid *>(IsAsteroid(a) ? a : (IsAsteroid(b) ? b : 0));
	MysteryShip *mysteryShip = static_cast<MysteryShip *>(a == mysteryShip_ ? a : (b == mysteryShip_ ? b : 0));

	if (bullet && player)
	{
		if(bullet->GetShooter() == Bullet::MYSTERY_SHIP)
		{
			PlayerHit();
			DeactivateBullet(bullet);
		}		
	}
	else if (player && mysteryShip)
	{
		PlayerHit();
		MysteryShipHit();
	}
	else if (bullet && mysteryShip)
	{
		if(bullet->GetShooter() == Bullet::PLAYER)
		{
			SetScore(100);
			float x = mysteryShip_->GetPosition().x + screenSize_.width/2 + Random::GetFloat(-25.0f, 25.0f);
			float y =  screenSize_.height/2 - mysteryShip_->GetPosition().y + Random::GetFloat(10.0f, 20.0f);
			hud_->ShowFloatingScore(100, static_cast<int>(x), static_cast<int>(y));
			MysteryShipHit();
			DeactivateBullet(bullet);
		}		
		
	}
	else if (player && asteroid)
	{
		PlayerHit();
		AsteroidHit(asteroid);
	}
	else if (bullet && asteroid)
	{
		if(bullet->GetShooter() == Bullet::PLAYER)
		{
			SetScore(GetAstroidHitScore(asteroid));
			float x = asteroid->GetPosition().x + screenSize_.width/2;
			float y =  screenSize_.height/2 - asteroid->GetPosition().y;
			hud_->ShowFloatingScore(GetAstroidHitScore(asteroid), static_cast<int>(x), static_cast<int>(y));
			AsteroidHit(asteroid);			
		}	
		DeactivateBullet(bullet);
	}

}

void Game::OnLeftDown(int x, int y)
{
	if(player_)
		player_->SetDestination(D3DXVECTOR3(static_cast<float>(x),static_cast<float>(y),0));
}

void Game::OnLeftUp(int x, int y)
{
}

void Game::OnRightDown(int x, int y)
{
	Fire();
}

void Game::OnRightUp(int x, int y)
{
}

void Game::SetScore(int score) const
{
	hud_->SetScore(score);
}

bool Game::CanPlayerSpawn() const
{
	return (hud_->GetLives() > 0 && explosions_.empty());
}

void Game::SpawnPlayer()
{
	DeletePlayer();
	player_ = new Ship();
	player_->EnableCollisions(collision_, 20.0f);
}

void Game::PlayerHit()
{
	if(mysteryShip_)
	{
		const long NEXT_FIRE_PLUS = 5000;
		mysteryShip_->SetNextFire(NEXT_FIRE_PLUS);
	}
	SpawnExplosion(player_->GetPosition(), 3, BLUE);
	hud_->SetLives(hud_->GetLives() - 1);
	DeletePlayer();
}

void Game::DeletePlayer()
{
	delete player_;
	player_ = 0;
}

void Game::UpdatePlayer(System *system)
{
	if (player_ == 0 )
	{
		if(CanPlayerSpawn())
		{
			SpawnPlayer();
		}
		else
		{
			return;
		}		
	}
		
	Keyboard *keyboard = system->GetKeyboard();
	if (keyboard->IsKeyPressed('B'))  //Switch firing style
	{
		if(player_)
		{
			int nextID = player_->GetBulletId() == 0 ? 1 : 0;		
			player_->SetBulletId(nextID); 
		}
	}

	player_->Update(system);

	if(clock() > player_->GetExhaustTimer())
	{
		ReleaseExhaust();
	}

	WrapEntity(player_);
}

void Game::ReleaseExhaust()
{
	const long EXHAUST_FREQUENCY = 100;
	const long NUM_PARTICLES = 5;
	player_->SetExhaustTimer(clock() + EXHAUST_FREQUENCY);
	D3DXVECTOR3 backward = -player_->GetForwardVector();
	D3DXVECTOR3 position = player_->GetPosition();
	D3DXVECTOR3 exhaustOrigin = position + backward * 10;
	
	int numOfParticles = NUM_PARTICLES;
	if(player_->GetAcceleration() == 0 )
	{
		numOfParticles = 1;
	}
	long randomLifeTime = static_cast<long>(Random::GetFloat(250, 1000));
	Exhaust * exhaust = new Exhaust(exhaustOrigin, randomLifeTime, numOfParticles, ORANGE);
	exhausts_.push_back(exhaust);
}

void Game::UpdateExhausts(System *system)
{
	
	ExhaustList::const_iterator exhaustIt  = exhausts_.begin();

	while(exhaustIt != exhausts_.end())
	{
		if(clock() - (*exhaustIt)->GetSpawnTime() >= (*exhaustIt)->GetLifeTime())
		{
			delete *exhaustIt;		
			exhaustIt = exhausts_.erase(exhaustIt);
		}
		else
		{
			WrapEntity(*exhaustIt);
			exhaustIt++;
		}
 	}
}

void Game::UpdateAsteroids(System *system)
{
	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
	++asteroidIt)
	{
		(*asteroidIt)->Update(system);
		WrapEntity(*asteroidIt);
	}
}

void Game::UpdateMysteryShip(System *system)
{
	if(mysteryShip_ &&  clock() >= mysteryShip_->GetSpawnTime())
	{
		if(clock() >= mysteryShip_->GetNextFireTime() && player_)
		{
			const long NEXT_FIRE = 3500;
			mysteryShip_->SetNextFire(NEXT_FIRE);
			D3DXVECTOR3 direction = player_->GetPosition() - mysteryShip_->GetPosition();
			D3DXVec3Normalize(&direction, &direction);
			D3DXVECTOR3 bulletPosition = mysteryShip_->GetPosition() + direction * 10.0f;
			SpawnBullet(1, 0, bulletPosition, direction);
		}

		if(mysteryShip_->GetPosition().x <= -screenSize_.width/2)
		{
			mysteryShip_->Reset();
		}
		else
		{
			mysteryShip_->Update(system);
			WrapEntity(mysteryShip_);
		}		
	}
}


void Game::UpdateBullet(System *system)
{
		for (BulletList::const_iterator bulletIt = bullets_.begin(),
		end = bullets_.end();
		bulletIt != end;
	++bulletIt)
	{
		if((*bulletIt)->GetIsActive())
		{
			//Bullet is deactivated afte it exits the screen
			if((*bulletIt)->GetPosition().x < -screenSize_.width/2 || (*bulletIt)->GetPosition().y < -screenSize_.height/2 || (*bulletIt)->GetPosition().x > screenSize_.width ||(*bulletIt)->GetPosition().y > screenSize_.height/2)
			{
				DeactivateBullet(*bulletIt);
			}
			else
			{		
				(*bulletIt)->Update(system); //Bullet will dissapear after exiting screen, so its not wrapped.			
			}
		}
	}
}

void Game::UpdateExplosions(System *system)
{
	
	ExplosionList::const_iterator exploisionIt  = explosions_.begin();
	const long EXPLOSION_LIFETIME = 2000;
	while(exploisionIt != explosions_.end())
	{
		if(clock() - (*exploisionIt)->GetSpawnTime() >= EXPLOSION_LIFETIME)
		{
			delete *exploisionIt;		
			exploisionIt = explosions_.erase(exploisionIt);
		}
		else
		{
			WrapEntity(*exploisionIt);
			exploisionIt++;
		}
 	}
}

void Game::WrapEntity(GameEntity *entity) const
{
	D3DXVECTOR3 entityPosition = entity->GetPosition();
	entityPosition.x = Maths::WrapModulo(entityPosition.x, -400.0f, 400.0f);
	entityPosition.y = Maths::WrapModulo(entityPosition.y, -300.0f, 300.0f);
	entity->SetPosition(entityPosition);
}

void Game::DeleteAllBullets()
{
	for (BulletList::const_iterator bulletIt = bullets_.begin(),
		end = bullets_.end();
		bulletIt != end;
		++bulletIt)
	{
		delete (*bulletIt);
	}

	bullets_.clear();
}

void Game::DeleteAllAsteroids()
{
	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
		++asteroidIt)
	{
		delete (*asteroidIt);
	}

	asteroids_.clear();
}

void Game::DeleteAllExplosions()
{
	for (ExplosionList::const_iterator explosionIt = explosions_.begin(),
		end = explosions_.end();
		explosionIt != end;
	++explosionIt)
	{
		delete (*explosionIt);
	}

	explosions_.clear();
}

void Game::DeleteAllExhausts()
{
	for (ExhaustList::const_iterator exhaustIt = exhausts_.begin(),
		end = exhausts_.end();
		exhaustIt != end;
	++exhaustIt)
	{
		delete (*exhaustIt);
	}

	exhausts_.clear();
}

void Game::Fire()
{
	if(clock() >= nextFire_ && player_)  // Fires only at the given rate.
	{
		D3DXVECTOR3 playerForward = player_->GetForwardVector();
		D3DXVECTOR3 bulletPosition = player_->GetPosition() + playerForward * 20.0f;

		SpawnBullet(0, player_->GetBulletId(), bulletPosition, playerForward);

		nextFire_ = clock() + rateOfFire_;

		player_->StopMoving(); // player stops accelerating when firing.
	}
}

void Game::SpawnBullet(int shooterID, int bulletTypeId, D3DXVECTOR3 position, D3DXVECTOR3 direction)
{
	SpawnBulletAt(shooterID, bulletTypeId, position, direction);

	if(bulletTypeId == 1) // Spawning two more bullets to left and right.
	{
		D3DXMATRIX rotationMatrix;
		D3DXVECTOR3 dir1;
		D3DXMatrixRotationZ(&rotationMatrix, Maths::PI/8);
		D3DXVec3TransformNormal(&dir1, &direction,
			&rotationMatrix);
		D3DXVec3Normalize(&dir1, &dir1);
		SpawnBulletAt(shooterID, bulletTypeId, position, dir1);

		D3DXVECTOR3 dir2;
		D3DXMatrixRotationZ(&rotationMatrix, -Maths::PI/8);
		D3DXVec3TransformNormal(&dir2, &direction,
			&rotationMatrix);
		D3DXVec3Normalize(&dir2, &dir2);
		SpawnBulletAt(shooterID, bulletTypeId, position, dir2);
	}
}

void Game::SpawnBulletAt(int shooterID, int bulletTypeId, const D3DXVECTOR3 &position, const D3DXVECTOR3 &direction)
{
	// Bullet is retreived from pool.
	Bullet * bullet = GetBulletFromPool();
	if(bullet)
	{
		ActivateBullet(shooterID, bullet, position, direction);
	}
}

bool Game::IsBullet(GameEntity *entity) const
{
	return (std::find(bullets_.begin(),
		bullets_.end(), entity) != bullets_.end()); 
}

void Game::ActivateBullet(int type, Bullet *bullet, const D3DXVECTOR3 &position, const D3DXVECTOR3 &direction)
{
	bullet->Initialise(type, position, direction);

	bullet->EnableCollisions(collision_, 3.0f);

	bullet->SetIsActive(true);

}

void Game::DeactivateBullet(Bullet *bullet)
{
	bullet->DisableCollisions();
	bullet->SetIsActive(false);
}


Bullet * Game::GetBulletFromPool()
{
	for (BulletList::const_iterator bulletIt = bullets_.begin(),
		end = bullets_.end();
		bulletIt != end;
		++bulletIt)
		{
 			if(!((*bulletIt)->GetIsActive()))
			{
				return *bulletIt; // An inactive bullet is retreived from the pool
			}
		}

		return CreateBullet(); // When no inactive bullets available, a new bullet is created
}


Bullet * Game::CreateBullet()
{
	Bullet *bullet = new Bullet(); 
	bullets_.push_back(bullet);
	return bullet;
}

void Game::SpawnAsteroids(int numAsteroids)
{
	float halfWidth = 800.0f * 0.5f;
	float halfHeight = 600.0f * 0.5f;
	for (int i = 0; i < numAsteroids; i++)
	{
		float x = 0; 
		// Spawning asteroids away from the player
		while(abs(x) < halfWidth/4)
		{
			x = Random::GetFloat(-halfWidth, halfWidth);
		}
		float y = 0; 
		while(abs(y) < halfHeight/4)
		{
			y = Random::GetFloat(-halfHeight, halfHeight);
		}
		D3DXVECTOR3 position = D3DXVECTOR3(x, y, 0.0f);
		SpawnAsteroidAt(position, 3);
	}

}

void Game::SpawnAsteroidAt(const D3DXVECTOR3 &position, int size)
{
	const float MAX_ASTEROID_SPEED = 1.0f;

	float angle = Random::GetFloat(Maths::TWO_PI);
	D3DXMATRIX randomRotation;
	D3DXMatrixRotationZ(&randomRotation, angle);
	D3DXVECTOR3 velocity = D3DXVECTOR3(0.0f, Random::GetFloat(MAX_ASTEROID_SPEED), 0.0f);
	D3DXVec3TransformNormal(&velocity, &velocity, &randomRotation);
	Asteroid *asteroid = new Asteroid(position, velocity, size);
	asteroid->EnableCollisions(collision_, size * 5.0f);
	asteroids_.push_back(asteroid);
}

bool Game::IsAsteroid(GameEntity *entity) const
{
	return (std::find(asteroids_.begin(),
		asteroids_.end(), entity) != asteroids_.end()); 
}

void Game::AsteroidHit(Asteroid *asteroid)
{
	int currentSize = asteroid->GetSize();
	if (currentSize > 1)
	{
		int smallerSize = currentSize - 1;
		D3DXVECTOR3 position = asteroid->GetPosition();
		SpawnAsteroidAt(position, smallerSize);
		SpawnAsteroidAt(position, smallerSize);		
	}

 	SpawnExplosion(asteroid->GetPosition(), currentSize, GREEN);
 	DeleteAsteroid(asteroid);
}

void Game::DeleteAsteroid(Asteroid *asteroid)
{
	asteroids_.remove(asteroid);
	delete asteroid;
}

int Game::GetAstroidHitScore(Asteroid *asteroid)
{
	int score = 0;

	switch (asteroid->GetSize())
	{
		case 3:
			score = 10;
			break;
		case 2:
			score = 20;
			break;
		case 1:
			score = 50;
		default:
			break;
	}

	return score;
}

void Game::SpawnExplosion(const D3DXVECTOR3 &position, int size, D3DCOLOR diffuse)
{
	Explosion *explosion = new Explosion(position, clock(), size, diffuse);
   	explosions_.push_back(explosion);
}

void Game::DeleteExplosion(Explosion *explosion)
{
	explosions_.remove(explosion);
	delete explosion;
}

void Game::SpawnMysteryShip()
{
	DeleteMysteryShip();
	
	float x = screenSize_.width/2;
	float y = screenSize_.height/2 - 50;
	D3DXVECTOR3 position = D3DXVECTOR3(x, y, 0.0f);

	mysteryShip_ = new MysteryShip(position);
	mysteryShip_->EnableCollisions(collision_, 25.0f);

}

void Game::MysteryShipHit()
{
	SpawnExplosion(mysteryShip_->GetPosition(), 4, RED);
	DeleteMysteryShip();
}

void Game::DeleteMysteryShip()
{
	if(mysteryShip_)
		mysteryShip_->DisableCollisions();
	delete mysteryShip_;
	mysteryShip_ = 0;
}

void Game::UpdateCollisions()
{
	collision_->DoCollisions(this);
}

void Game::UpdateHUD(System *system)
{
	if (hud_)
	{
		hud_->Update(system);
	}
}