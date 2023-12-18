#include "cameraShake.h"
#include "utils.h"

static float shake = 0.0f;

void doShake()
{
	PlaydateAPI* pd = getPlaydateAPI();

	int shakeX = 32 - randomInt(0, 64);
	int shakeY = 32 - randomInt(0, 64);

	pd->graphics->setDrawOffset(shakeX * shake, shakeY * shake);

	shake = shake * 0.95f;
	if (shake < 0.05f)
	{
		shake = 0.0f;
	}
}

void setShake(float value)
{
	shake = value;
}

float getShake()
{
	return shake;
}
