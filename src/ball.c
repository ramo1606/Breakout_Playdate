#include "ball.h"
#include "memory.h"
#include "common.h"

#include <stdbool.h>

struct BallData
{
	float dx;
	float dy;
	float speed;
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

	pd->sprite->moveTo(ball, x, y);
	pd->sprite->setZIndex(ball, 1000);
	pd->sprite->addSprite(ball);

	pd->sprite->setTag(ball, BALL);

	// Initialize paddle data
	BallData* ball_data = pd_malloc(sizeof(BallData));
	ball_data->dx = BALL_DX;
	ball_data->dy = BALL_DY;
	ball_data->speed = BALL_SPEED;
	pd->sprite->setUserdata(ball, (void*)ball_data);
	return ball;
}

void ball_update_sprite(LCDSprite* sprite)
{
	if (sprite)
	{
		PlaydateAPI* pd = get_playdate_API();
		BallData* ball_data = (BallData*)pd->sprite->getUserdata(sprite);
		if (ball_data)
		{
			float x = 0;
			float y = 0;
			pd->sprite->getPosition(sprite, &x, &y);
			int nextx = x + ball_data->dx * ball_data->speed;
			int nexty = y + ball_data->dy * ball_data->speed;

			PDRect ball_rect = pd->sprite->getCollideRect(sprite);
			if (nextx + ball_rect.width / 2 >= pd->display->getWidth())
			{
				pd->sprite->moveTo(sprite, pd->display->getWidth() - ball_rect.width / 2, y);
				ball_set_dx(sprite, -abs(ball_data->dx));
			}
			if (nextx - ball_rect.width / 2 <= 0)
			{
				pd->sprite->moveTo(sprite, 0 + ball_rect.width / 2, y);
				ball_set_dx(sprite, abs(ball_data->dx));
			}
			if (nexty <= 0 + ball_rect.height / 2)
			{
				ball_set_dy(sprite, abs(ball_data->dy));
				pd->sprite->moveTo(sprite, x, 0 + ball_rect.height / 2);
			}
			if (nexty >= pd->display->getHeight() - ball_rect.height / 2)
			{
				ball_set_dy(sprite, -abs(ball_data->dy));
				pd->sprite->moveTo(sprite, x, pd->display->getHeight() - ball_rect.height / 2);
			}

			int len = 0;
			float actual_x = 0;
			float actual_y = 0;
			SpriteCollisionInfo* collisions = NULL;
			collisions = pd->sprite->moveWithCollisions(sprite, nextx, nexty, &actual_x, &actual_y, &len);

			if(collisions)
			{
				ball_set_dy(sprite, -abs(ball_data->dy));
			}
		}
	}
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
	return kCollisionTypeBounce;
}
