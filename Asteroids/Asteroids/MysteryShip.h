#ifndef MYSTERY_SHIP_H_INCLUDED
#define MYSTERY_SHIP_H_INCLUDED

#include "GameEntity.h"
class Graphics;

class MysteryShip : public GameEntity
{
public:
	
	MysteryShip(const D3DXVECTOR3 &position);

	void Update(System *system);
	void Render(Graphics *graphics) const;

	void  Reset();
	void  SetNextFire(const long delay);

	long GetSpawnTime() const{return spawnTime_;}
	long GetNextFireTime() const {return nextFireTime_;}

private:

	D3DXVECTOR3 velocity_;
	float rotation_;
	long spawnTime_;
	D3DXVECTOR3 spawnPoint_;
	long nextFireTime_;

};

#endif // MYSTERY_SHIP_H_INCLUDED
