#include "Exhaust.h"
#include "System.h"
#include "Graphics.h"
#include "Random.h"
#include <time.h>
#include "Maths.h"

Exhaust::Exhaust(const D3DXVECTOR3 &position, const long lifeTime, const int count, D3DCOLOR diffuse):lifeTime_(lifeTime)
{
	particleCount_ = count;
	particles_.reserve(particleCount_);

	for (int i = 0; i < particleCount_; i++)
	{
		ParticleVertex p;
		p.x = Random::GetFloat(position.x - 5, position.x + 5);
		p.y = Random::GetFloat(position.y - 5, position.y + 5);
		p.z = 100.0f;
		p.diffuse = diffuse;
		particles_.push_back(p);
	}

	spawnTime_ = clock();
}

Exhaust::~Exhaust()
{
	particles_.clear();
}

void Exhaust::Update(System *systems)
{

}

void Exhaust::Render(Graphics *graphics) const
{
	DWORD exhFvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	graphics->SetVertexFormat(exhFvf);
	graphics->SetPointSize(1.5f);
	graphics->DisableLighting();
	graphics->DrawImmediate(D3DPT_POINTLIST,
		particleCount_,
		&particles_[0],
		sizeof(particles_[0]));
	graphics->EnableLighting();
}