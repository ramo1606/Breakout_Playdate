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

	pd->sprite->setTag(brick, BRICK);

	// Initialize paddle data
	BrickData* brick_data = (BrickData*)pd_malloc(sizeof(BrickData));
	brick_data->dx = 0;
	brick_data->dy = 0;
	brick_data->hp = 1;
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

		//float x = 0;
		//float y = 0;
		//pd->sprite->getPosition(sprite, &x, &y);
		//
		//float actual_x = 0.f;
		//float actual_y = 0.f;
		//int len = 0;
		//
		//SpriteCollisionInfo* collisions = NULL;
		//collisions = pd->sprite->checkCollisions(sprite, x, y, &actual_x, &actual_y, &len);
		//
		//if (len == 1) 
		//{
		//	uint8_t other_tag = pd->sprite->getTag(collisions[0].other);
		//	if (other_tag == BALL) 
		//	{
		//		pd->sprite->removeSprite(sprite);
		//		brick_destroy(sprite);
		//	}
		//}
		//BrickData* brick_data = (BrickData*)pd->sprite->getUserdata(sprite);
		//if (brick_data)
		//{
		//	
		//}
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
	return kCollisionTypeBounce;
}
