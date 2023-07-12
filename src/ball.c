#include "ball.h"
#include "memory.h"
#include "common.h"

#include <stdbool.h>

struct BallData
{
	float dx;
	float dy;
};

LCDSprite* ball_create(float x, float y, LCDBitmap* image)
{
	PlaydateAPI* pd = get_playdate_API();

	LCDSprite* ball = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(ball, ball_update_sprite);
	pd->sprite->setImage(ball, image, kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(image, &w, &h, NULL, NULL, NULL);

	// Create collision rect for ball
	PDRect cr = PDRectMake(0, 0, (float)w, (float)h);
	pd->sprite->setCollideRect(ball, cr);
	pd->sprite->setCollisionResponseFunction(ball, ball_collision_response);

	pd->sprite->moveTo(ball, (float)x, (float)y);
	pd->sprite->setZIndex(ball, 1000);
	pd->sprite->addSprite(ball);

	pd->sprite->setTag(ball, BALL);

	// Initialize paddle data
	BallData* ball_data = pd_malloc(sizeof(BallData));
	ball_data->dx = 0.f;
	ball_data->dy = 0.f;
	pd->sprite->setUserdata(ball, (void*)ball_data);
	return ball;
}

void ball_update_sprite(LCDSprite* sprite)
{
}

void ball_set_dx(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			ball_data->dx = value;
		}
	}
}

float ball_get_dx(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			return ball_data->dx;
		}
	}
}

void ball_set_dy(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			ball_data->dy = value;
		}
	}
}

float ball_get_dy(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			return ball_data->dy;
		}
	}
}

SpriteCollisionResponseType ball_collision_response(LCDSprite* sprite, LCDSprite* other)
{
}
