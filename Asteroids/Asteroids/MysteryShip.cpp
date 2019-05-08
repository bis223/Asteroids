#include "MysteryShip.h"
#include "Graphics.h"
#include "Maths.h"
#include "time.h"
#include "Random.h"

MysteryShip::MysteryShip(const D3DXVECTOR3 &position) : 
	velocity_(D3DXVECTOR3(-1.0f, 0.0f, 0.0f)),
	rotation_(Maths::PI/2),
	spawnTime_(0)
{

	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationZ(&rotationMatrix, rotation_);
	spawnPoint_ = position;
	
	Reset();
}


void MysteryShip::Update(System *system)
{
	D3DXVECTOR3 newPosition = GetPosition();
	D3DXVec3Add(&newPosition, &newPosition, &velocity_);
	SetPosition(newPosition);
}

void MysteryShip::Render(Graphics *graphics) const
{
	struct DummyVert
	{
		float x, y, z;
		D3DCOLOR diffuse;
	};

	DummyVert axis[18] =
	{
		{-5.0f, -25.0f, 0.0f, 0xffffffff}, {-5.0f, 25.0f, 0.0f, 0xffffffff},
		{-5.0f, -25.0f, 0.0f, 0xffffffff}, {5.0f, -5.0f, 0.0f, 0xffffffff},
		{-5.0f, 25.0f, 0.0f, 0xffffffff}, {5.0f, 5.0f, 0.0f, 0xffffffff},
		{5.0f, -5.0f, 0.0f, 0xffffffff}, {5.0f, 5.0f, 0.0f, 0xffffffff},
		{-5.0f, -20.0f, 0.0f, 0xffffffff}, {-10.0f, -20.0f, 0.0f, 0xffffffff},
		{-5.0f, -10.0f, 0.0f, 0xffffffff}, {-10.0f, -10.0f, 0.0f, 0xffffffff},
		{-5.0f, 0.0f, 0.0f, 0xffffffff}, {-10.0f, 0.0f, 0.0f, 0xffffffff},
		{-5.0f, 10.0f, 0.0f, 0xffffffff}, {-10.0f, 10.0f, 0.0f, 0xffffffff},
		{-5.0f, 20, 0.0f, 0xffffffff}, {-10.0f, 20.0f, 0.0f, 0xffffffff}
	};

	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationZ(&rotationMatrix, rotation_);

	D3DXVECTOR3 position = GetPosition();
	D3DXMATRIX translationMatrix;
	D3DXMatrixTranslation(&translationMatrix,
		position.x,
		position.y,
		position.z);

	D3DXMATRIX shipTransform = rotationMatrix * translationMatrix;

	D3DXMATRIX identityMatrix;
	D3DXMatrixIdentity(&identityMatrix);

	DWORD dummyFvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	graphics->SetVertexFormat(dummyFvf);
	graphics->DisableLighting();
	graphics->SetModelMatrix(&shipTransform);
	graphics->DrawImmediate(D3DPT_LINELIST,
		9,
		&axis[0],
		sizeof(axis[0]));
	graphics->SetModelMatrix(&identityMatrix);
	graphics->EnableLighting();
}

void MysteryShip::Reset()
{
	const long DELAY = 10000;
	spawnTime_ = clock() + DELAY;
	SetPosition(spawnPoint_);

	nextFireTime_ = spawnTime_ + 5000;
}

void MysteryShip::SetNextFire(const long delay)
{
	nextFireTime_ = clock() + delay;
}
