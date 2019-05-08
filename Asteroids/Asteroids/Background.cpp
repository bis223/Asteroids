#include "Background.h"
#include "System.h"
#include "Graphics.h"
#include "Random.h"

Background::Background(float width, float height)
{
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;
	for (int i = 0; i < NUM_STARS; i++)
	{
		stars_[i].x = Random::GetFloat(-halfWidth, halfWidth);
		stars_[i].y = Random::GetFloat(-halfHeight, halfHeight);
		stars_[i].z = 100.0f;
		stars_[i].diffuse = 0xffffffff;
	}
}

void Background::Update(System *systems)
{
}

void Background::Render(Graphics *graphics) const
{
	graphics->ClearFrame(0x00000000);

	DWORD starFvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	graphics->SetVertexFormat(starFvf);
	graphics->SetPointSize(1.0f); //Reduced size from 1.5f to 1.0f becasue its nicer!!
	graphics->DisableLighting();
	graphics->DrawImmediate(D3DPT_POINTLIST,
		NUM_STARS,
		&stars_[0],
		sizeof(stars_[0]));
	graphics->EnableLighting();
}
