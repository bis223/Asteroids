#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "GameEntity.h"

class Bullet : public GameEntity
{
public:
	enum ShooterType {PLAYER = 0, MYSTERY_SHIP = 1}; // Two type of bullets based on the shooters.
	Bullet();

	void Update(System *system);
	void Render(Graphics *graphics) const;

	void Initialise(int bulletType, const D3DXVECTOR3 &position, const D3DXVECTOR3 &direction);
	void SetIsActive(bool isActive);

	ShooterType GetShooter() const { return shooterType_; }
	bool        GetIsActive() const { return isActive_; }

private:

	D3DXVECTOR3 velocity_;
	bool		isActive_;	
	ShooterType	shooterType_;
};

#endif // BULLET_H_INCLUDED
