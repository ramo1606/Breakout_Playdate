#include "cameraShake.h"

#include "common.h"
#include "utils.h"

static float shake = 0.0f;

void SHAKE_update()
{
	int shakeX = 42 - randomInt(0, 84);
	int shakeY = 42 - randomInt(0, 84);

	pd->graphics->setDrawOffset(shakeX * shake, shakeY * shake);

	shake = shake * 0.95f;
	if (shake < 0.05f)
	{
		shake = 0.0f;
	}
}

void SHAKE_set(float value)
{
	shake = value;
}

float SHAKE_get()
{
	return shake;
}
