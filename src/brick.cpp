#include "brick.h"
#include "memory.h"
#include "common.h"

#include <stdbool.h>

struct BrickData
{
	float dx;
	float dy;
	float hp;
};

LCDSprite* brick_create(float x, float y, LCDBitmap* image)
{
	PlaydateAPI* pd = get_playdate_API();

	LCDSprite* brick = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(brick, brick_update_sprite);
	pd->sprite->setImage(brick, image, kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(image, &w, &h, NULL, NULL, NULL);

	// Create collision rect for ball
	PDRect cr = PDRectMake(0, 0, (float)w, (float)h);
	pd->sprite->setCollideRect(brick, cr);
	pd->sprite->setCollisionResponseFunction(brick, brick_collision_response);

	pd->sprite->moveTo(brick, x, y);
	pd->sprite->setZIndex(brick, 1000);
	pd->sprite->addSprite(brick);

	pd->sprite->setTag(brick, BALL);

	// Initialize paddle data
	BrickData* brick_data = (BrickData*)pd_malloc(sizeof(BrickData));
	brick_data->dx = BALL_DX;
	brick_data->dy = BALL_DY;
	brick_data->hp = BRICK_HP;
	pd->sprite->setUserdata(brick, (void*)brick_data);
	return brick;
}

void brick_destroy(LCDSprite* sprite)
{
	BrickData* brick_data = (BrickData*)get_playdate_API()->sprite->getUserdata(sprite);
	pd_free(brick_data);
	pd_free(sprite);
}

void brick_update_sprite(LCDSprite* sprite)
{
	if (sprite)
	{
		PlaydateAPI* pd = get_playdate_API();
		BrickData* brick_data = (BrickData*)pd->sprite->getUserdata(sprite);
		if (brick_data)
		{
			
		}
	}
}

void brick_set_dx(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BrickData* brick_data = (BrickData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (brick_data)
		{
			brick_data->dx = value;
		}
	}
}

float brick_get_dx(LCDSprite* sprite)
{
	if (sprite)
	{
		BrickData* brick_data = (BrickData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (brick_data)
		{
			return brick_data->dx;
		}
	}
}

void brick_set_dy(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BrickData* brick_data = (BrickData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (brick_data)
		{
			brick_data->dx = value;
		}
	}
}

float brick_get_dy(LCDSprite* sprite)
{
	if (sprite)
	{
		BrickData* brick_data = (BrickData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (brick_data)
		{
			return brick_data->dx;
		}
	}
}

SpriteCollisionResponseType brick_collision_response(LCDSprite* sprite, LCDSprite* other)
{
	return SpriteCollisionResponseType();
}
