#ifndef EXPLOSION_H_INCLUDED
#define EXPLOSION_H_INCLUDED

#include "GameEntity.h"
#include <vector>
class Explosion : public GameEntity
{
public:

	Explosion(const D3DXVECTOR3 &position, const long spawnTime, const int size, D3DCOLOR diffuse);
	~Explosion();
	void Update(System *system);
	void Render(Graphics *graphics) const;

	long GetSpawnTime() const {return spawnTime_;}

private:

	struct ParticleVertex
	{
		float x, y, z;
		D3DCOLOR diffuse;
	};

	std::vector<ParticleVertex> particles_;
	long spawnTime_;
	int particleCount_;
};

#endif // EXPLOSION_H_INCLUDED
