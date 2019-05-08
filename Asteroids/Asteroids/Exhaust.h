#ifndef EXHAUST_H_INCLUDED
#define EXHAUST_H_INCLUDED

#include "GameEntity.h"
#include <vector>
class Exhaust : public GameEntity
{
public:

	Exhaust(const D3DXVECTOR3 &position, const long lifeTime, const int count, D3DCOLOR diffuse);
	~Exhaust();
	void Update(System *system);
	void Render(Graphics *graphics) const;

	long GetSpawnTime() const {return spawnTime_;}
	long GetLifeTime() const {return lifeTime_;}

private:

	struct ParticleVertex
	{
		float x, y, z;
		D3DCOLOR diffuse;
	};

	std::vector<ParticleVertex> particles_;
	long spawnTime_;
	long lifeTime_;
	int  particleCount_;
};

#endif // EXHAUST_H_INCLUDED
