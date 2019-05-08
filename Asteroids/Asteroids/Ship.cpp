#include "Ship.h"
#include "Graphics.h"
#include "Maths.h"
#include <algorithm>
#include "Random.h"
#include <time.h>

Ship::Ship() :
	accelerationControl_(0.0f),
	velocity_(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	forward_(D3DXVECTOR3(0.0f, 1.0f, 0.0f)),
	destination_(D3DXVECTOR3(0.0f, 1.0f, 0.0f)),
	rotation_(0.0f) //**TODO: Candidate for crash
{
	exhaustTimer_ = clock() + 100;
	chosenBulletTypeId_ = 0;
	Reset();
}

Ship::~Ship()
{
}

void Ship::StopMoving()
{
	accelerationControl_ = 0;
}

void Ship::SetControlInput(float acceleration,
	float rotation)
{
	accelerationControl_ = acceleration;
	//rotationControl_ = rotation;
}

void Ship::SetDestination(D3DXVECTOR3 pos)
{
	destination_ = pos;
	float x = pos.x - 400;
	float y =  300 - pos.y;
	destination_  = D3DXVECTOR3(x, y, 0);
	accelerationControl_ = 1;
}

void Ship::Update(System *system)
{
	const float RATE_OF_ROTATION = 0.08f; 
	const float MAX_SPEED = 2.0f;
	const float VELOCITY_TWEEN = 0.05f;
	

	D3DXVECTOR3 vectToDestination;
	D3DXVec3Subtract(&vectToDestination, &destination_, &GetPosition());
	float distance = D3DXVec3Length(&vectToDestination);
	if(accelerationControl_ > 0 && distance < 50.0f)
	{
		accelerationControl_ = 0;
	}

	D3DXVec3Normalize(&vectToDestination, &vectToDestination);
	float dot =  D3DXVec3Dot(&vectToDestination, &forward_); 
	float det = vectToDestination.x * forward_.y - vectToDestination.y * forward_.x;
	float newRotation = acos(dot); // finding angle need to rotate towards destination.
	float nextRot = rotation_;
	if(atan2(det, dot) > 0) // finding the side to which the rotation should happen
	{
		nextRot -= newRotation;
	}
	else
	{
		nextRot += newRotation;
	}

	//Tweening the rotation
	if(rotation_ < nextRot)
	{
		rotation_ += RATE_OF_ROTATION;
		if (rotation_ >= nextRot)
		{
			rotation_ = nextRot;
			nextRot = 0;
		}
	}
	else
	{
		rotation_ -= RATE_OF_ROTATION;
		if (rotation_ <= nextRot)
		{
			rotation_ = nextRot;
			nextRot = 0;
		}
	}

	rotation_ = Maths::WrapModulo(rotation_, Maths::TWO_PI);	
	
	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationZ(&rotationMatrix, rotation_);
	D3DXVec3TransformNormal(&forward_, &D3DXVECTOR3(0.0f, 1.0f, 0.0f),
	&rotationMatrix);

	D3DXVec3Normalize(&forward_, &forward_);

	D3DXVECTOR3 idealVelocity;
	D3DXVec3Scale(&idealVelocity, &forward_, accelerationControl_ * MAX_SPEED);
	D3DXVec3Lerp(&velocity_, &velocity_, &idealVelocity, VELOCITY_TWEEN);

	D3DXVECTOR3 newPosition = GetPosition();
	D3DXVec3Add(&newPosition, &newPosition, &velocity_);
	SetPosition(newPosition);
}

void Ship::Render(Graphics *graphics) const
{
	
	struct DummyVert
	{
		float x, y, z;
		D3DCOLOR diffuse;
	};

	DummyVert axis[12] =
	{
		{-12.0f, 0.0f, 0.0f, 0xffffffff}, {12.0f, 0.0f, 0.0f, 0xffffffff},
		{0.0f, 20.0f, 0.0f, 0xffffffff}, {-12.0f, 0.0f, 0.0f, 0xffffffff},
		{0.0f, 20.0f, 0.0f, 0xffffffff}, {12.0f, 0.0f, 0.0f, 0xffffffff},
		{5.0f, 0.0f, 0.0f, 0xffffffff}, {5.0f, -5.0f, 0.0f, 0xffffffff},
		{-5.0f, 0.0f, 0.0f, 0xffffffff}, {-5.0f, -5.0f, 0.0f, 0xffffffff},
		{5.0f, -5.0f, 0.0f, 0xffffffff}, {-5.0f, -5.0f, 0.0f, 0xffffffff}
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
		6,
		&axis[0],
		sizeof(axis[0]));
	graphics->SetModelMatrix(&identityMatrix);
	graphics->EnableLighting();
}

D3DXVECTOR3 Ship::GetForwardVector() const
{
	return forward_;
}

D3DXVECTOR3 Ship::GetVelocity() const
{
	return velocity_;
}

void Ship::Reset()
{
	accelerationControl_ = 0.0f;

	velocity_ = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	forward_ = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	rotation_ = 0.0f;

	SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

}