#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "GameEntity.h"
#include <list>
class Graphics;

class Ship : public GameEntity
{
public:
	Ship();
	~Ship();

	void SetControlInput(float acceleration, float rotation);
	void StopMoving();
	void SetDestination(D3DXVECTOR3 pos);

	void Update(System *system);
	void Render(Graphics *graphics) const;

	D3DXVECTOR3 GetForwardVector() const;
	D3DXVECTOR3 GetVelocity() const;
	long		GetExhaustTimer() const {return exhaustTimer_;}
	float		GetAcceleration() const {return accelerationControl_;}
	int 		GetBulletId()const { return chosenBulletTypeId_;}

	void SetExhaustTimer(const long timer) { exhaustTimer_ = timer;}
	void SetBulletId(const int id) { chosenBulletTypeId_ = id;}
	void Reset();
	
private:

	float accelerationControl_;
	float rotation_;

	D3DXVECTOR3 velocity_;
	D3DXVECTOR3 forward_;
	D3DXVECTOR3 destination_;
	struct ExhaustVertex
	{
		float x, y, z;
		D3DCOLOR diffuse_;
		long lifeTime_;	
	};
	long exhaustTimer_;
	int chosenBulletTypeId_;
};

#endif // SHIP_H_INCLUDED
