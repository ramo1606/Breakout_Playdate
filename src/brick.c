#include "brick.h"
#include "memory.h"
#include "utils.h"
#include "resourcemanager.h"

#include <stdbool.h>

#define BRICK_WIDTH_SPACE_OFFSET 6
#define BRICK_HEIGHT_SPACE_OFFSET 4

static const float BRICK_HP = 1.f;

struct BrickData
{
	bool v;
	float dx;
	float dy;
	float hp;
	EBrickType type;
	int ox;
	int oy;
	int fsh;
};

LCDSprite* BRICK_create(int gridPos, char type)
{
	PlaydateAPI* pd = getPlaydateAPI();

	// Create new Sprite
	LCDSprite* brick = pd->sprite->newSprite();

	// Set update function
	pd->sprite->setUpdateFunction(brick, BRICK_updateSprite);

	// Set bitmap
	char* brickName = NULL; // temporary string
	getPlaydateAPI()->system->formatString(&brickName, "brick_%c", type);
	pd->sprite->setImage(brick, RESOURCEMANAGER_getImage(brickName), kBitmapUnflipped);
	
	// Get width and height of the bitmap
	int w, h;
	pd->graphics->getBitmapData(RESOURCEMANAGER_getImage(brickName), &w, &h, NULL, NULL, NULL);

	// Create bounds rect for ball
	PDRect bounds = PDRectMake(0.f, 0.f, (float)w, (float)h);
	pd->sprite->setBounds(brick, bounds);

	// Create collision rect for ball
	PDRect cr = PDRectMake(1.f, 1.f, (float)(w - 2), (float)(h - 2));
	pd->sprite->setCollideRect(brick, cr);
	pd->sprite->setCollisionResponseFunction(brick, BRICK_collisionResponse);

	// Calculate position
	float x = 80 + ((gridPos - 1) % 11) * (w + BRICK_WIDTH_SPACE_OFFSET);
	float y = 10 + (int)floor((gridPos - 1) / 11) * (h + BRICK_HEIGHT_SPACE_OFFSET);
	pd->sprite->moveTo(brick, x, y);
	pd->sprite->setZIndex(brick, 1000);
	pd->sprite->setTag(brick, BRICK);
	pd->sprite->addSprite(brick);

	// Initialize paddle data
	BrickData* brickData = pd_malloc(sizeof(BrickData));
	brickData->v = true;
	brickData->dx = 0.f;
	brickData->dy = randomFloat(0.f, 64.f);
	brickData->type = BRICK_translateType(type);
	brickData->hp = (brickData->type == HARDENED) ? BRICK_HP * 2 : BRICK_HP;
	brickData->ox = 0;
	brickData->oy = 128+randomInt(0, 128);
	brickData->fsh = 0;
	pd->sprite->setUserdata(brick, (void*)brickData);

	// Remove temporary string
	pd_free(brickName);
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
