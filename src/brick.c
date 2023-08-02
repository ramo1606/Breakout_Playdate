#include "brick.h"
#include "memory.h"

#include <stdbool.h>

static const float BRICK_HP = 1.f;

struct BrickData
{
	float dx;
	float dy;
	float hp;
	EBrickType type;
};

LCDSprite* BRICK_create(float x, float y, LCDBitmap* image, EBrickType type)
{
	PlaydateAPI* pd = getPlaydateAPI();

	LCDSprite* brick = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(brick, BRICK_updateSprite);
	pd->sprite->setImage(brick, image, kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(image, &w, &h, NULL, NULL, NULL);

	// Create collision rect for ball
	PDRect cr = PDRectMake(0, 0, (float)w, (float)h);
	pd->sprite->setCollideRect(brick, cr);
	pd->sprite->setCollisionResponseFunction(brick, BRICK_collisionResponse);

	pd->sprite->moveTo(brick, x, y);
	pd->sprite->setZIndex(brick, 1000);
	pd->sprite->addSprite(brick);

	pd->sprite->setTag(brick, BRICK);

	// Initialize paddle data
	BrickData* brickData = pd_malloc(sizeof(BrickData));
	brickData->dx = 0;
	brickData->dy = 0;
	brickData->hp = BRICK_HP;
	brickData->type = type;
	pd->sprite->setUserdata(brick, (void*)brickData);
	return brick;
}

void BRICK_destroy(LCDSprite* sprite)
{
	getPlaydateAPI()->sprite->removeSprite(sprite);
	BrickData* brick_data = (BrickData*)getPlaydateAPI()->sprite->getUserdata(sprite);
	pd_free(brick_data);
	pd_free(sprite);
}

void BRICK_updateSprite(LCDSprite* sprite)
{
	if (sprite)
	{
		PlaydateAPI* pd = getPlaydateAPI();

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

void BRICK_setDx(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BrickData* brick_data = (BrickData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (brick_data)
		{
			brick_data->dx = value;
		}
	}
}

float BRICK_getDx(LCDSprite* sprite)
{
	if (sprite)
	{
		BrickData* brick_data = (BrickData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (brick_data)
		{
			return brick_data->dx;
		}
	}
}

void BRICK_setDy(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BrickData* brick_data = (BrickData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (brick_data)
		{
			brick_data->dx = value;
		}
	}
}

float BRICK_getDy(LCDSprite* sprite)
{
	if (sprite)
	{
		BrickData* brick_data = (BrickData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (brick_data)
		{
			return brick_data->dx;
		}
	}
}

EBrickType BRICK_getType(LCDSprite* sprite)
{
	if (sprite)
	{
		BrickData* brick_data = (BrickData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (brick_data)
		{
			return brick_data->type;
		}
	}
}

EBrickType BRICK_translateType(char type)
{
	EBrickType returnType = EMPTY;
	if (type == 'b')
		returnType = REGULAR;
	else if (type == 'i')
		returnType = INVINCIBLE;
	else if (type == 'h')
		returnType = HARDENED;
	else if (type == 's')
		returnType = SPLODING;
	else if (type == 'p')
		returnType = POWER;

	return returnType;
}

SpriteCollisionResponseType BRICK_collisionResponse(LCDSprite* sprite, LCDSprite* other)
{
	return kCollisionTypeBounce;
}
