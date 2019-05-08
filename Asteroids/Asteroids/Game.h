#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <d3dx9math.h>
#include <list>
#include <memory>
class OrthoCamera;
class Background;
class Ship;
class Bullet;
class Asteroid;
class Explosion;
class Collision;
class System;
class Graphics;
class GameEntity;
class HUD;
class MysteryShip;
class Exhaust;

class Game
{
public:
	Game();
	~Game();

	void Update(System *system);
	void RenderBackgroundOnly(Graphics *graphics);
	void RenderEverything(Graphics *graphics);

	void InitialiseLevel(int level);
	bool IsLevelComplete() const;
	bool IsGameOver() const;
	void GameOver();
	std::list<std::shared_ptr<int>> GetHighScoreList();
	void DoCollision(GameEntity *a, GameEntity *b);

	void OnLeftDown(int x, int y);
	void OnLeftUp(int x, int y);
	void OnRightDown(int x, int y);
	void OnRightUp(int x, int y);

	struct ScreenSize
	{
		float width;
		float height;
	};
	ScreenSize GetScreenSize() const { return screenSize_; }
	
private:
	Game(const Game &);
	void operator=(const Game &);
	typedef std::list<Bullet *> BulletList;
	typedef std::list<Asteroid *> AsteroidList;
	typedef std::list<Explosion *> ExplosionList;
	typedef std::list<Exhaust *> ExhaustList;
	
	void SetScore(int score) const;
	bool CanPlayerSpawn() const;
	void SpawnPlayer();
	void PlayerHit();
	void DeletePlayer();

	void UpdatePlayer(System *system);
	void UpdateAsteroids(System *system);
	void UpdateMysteryShip(System *system);
	void UpdateBullet(System *system);
	void UpdateExplosions(System *system);
	void WrapEntity(GameEntity *entity) const;
	void ReleaseExhaust();
	void UpdateExhausts(System *system);

	void DeleteAllBullets();
	void DeleteAllAsteroids();
	void DeleteAllExplosions();
	void DeleteAllExhausts();

	void Fire();
	void SpawnBullet(int shooterID, int bulletTypeId, D3DXVECTOR3 position, D3DXVECTOR3 direction);
	void SpawnBulletAt(int shooterID, int bulletTypeId, const D3DXVECTOR3 &position, const D3DXVECTOR3 &direction);
	bool IsBullet(GameEntity *entity) const;
	void ActivateBullet(int type, Bullet * bullet, const D3DXVECTOR3 &position, const D3DXVECTOR3 &direction);
	void DeactivateBullet(Bullet * bullet);
	Bullet *GetBulletFromPool();
	Bullet *CreateBullet();

	void SpawnAsteroids(int numAsteroids);
	void SpawnAsteroidAt(const D3DXVECTOR3 &position, int size);
	bool IsAsteroid(GameEntity *entity) const;
	void AsteroidHit(Asteroid *asteroid);
	void DeleteAsteroid(Asteroid *asteroid);
	int  GetAstroidHitScore(Asteroid *asteroid);

	void SpawnExplosion(const D3DXVECTOR3 &position, int size, D3DCOLOR diffuse);
	void DeleteExplosion(Explosion *explosion);

	void SpawnMysteryShip();
	void MysteryShipHit();
	void DeleteMysteryShip();

	void UpdateCollisions();
	void UpdateHUD(System *system);

	OrthoCamera *camera_;

	Background *background_;
	Ship *player_;
	MysteryShip *mysteryShip_;
	BulletList bullets_;
	AsteroidList asteroids_;
	ExplosionList explosions_;
	ExhaustList exhausts_;

	Collision *collision_;

	HUD *hud_;

	ScreenSize screenSize_;

	long nextFire_;
	long rateOfFire_;
};

#endif // GAME_H_INCLUDED
