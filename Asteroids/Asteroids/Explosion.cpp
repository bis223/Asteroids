#include "Explosion.h"
#include "System.h"
#include "Graphics.h"
#include "Random.h"
#include <time.h>
#include "Maths.h"

Explosion::Explosion(const D3DXVECTOR3 &position, const long spawnTime, const int size, D3DCOLOR diffuse):spawnTime_(spawnTime)
{
	const float SCALE = 8.0f;
	const int NUM_PARTICLES = 20;

	float radius = SCALE * static_cast<float>(size);
	particleCount_ = NUM_PARTICLES * size;
	particles_.reserve(particleCount_);

 	for (int i = 0; i < particleCount_; i++)
	{
		float rand = Random::GetFloat(0, radius);
		float angle = Maths::WrapModulo(rand, Maths::TWO_PI);
		ParticleVertex p;
		p.x = position.x + Random::GetFloat(0, radius) * cos(angle);
		p.y = position.y + Random::GetFloat(0, radius) * sin(angle);
		p.z = 100.0f;
		p.diffuse = diffuse;
		particles_.push_back(p);
	}

	spawnTime_ = spawnTime;
}

Explosion::~Explosion()
{
	particles_.clear();
}

void Explosion::Update(System *systems)
{
}

void Explosion::Render(Graphics *graphics) const
{
	DWORD expFvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	graphics->SetVertexFormat(expFvf);
	graphics->SetPointSize(1.5f);
	graphics->DisableLighting();
	graphics->DrawImmediate(D3DPT_POINTLIST,
		particleCount_,
		&particles_[0],
		sizeof(particles_[0]));
	graphics->EnableLighting();
}