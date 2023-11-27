#include "brick.h"
#include "memory.h"
#include "rmem.h"
#include "raymath.h"
#include "utils.h"
#include "resourcemanager.h"
#include "engine.h"
#include "easing.h"

#include <stdbool.h>

#define BRICK_WIDTH_SPACE_OFFSET 3
#define BRICK_HEIGHT_SPACE_OFFSET 2

static const int BRICK_HP = 1;

void animate(LCDSprite* sprite) 
{
	PlaydateAPI* pd = getPlaydateAPI();

	BrickData* brickData = (BrickData*)pd->sprite->getUserdata(sprite);
	if (brickData->animationElapsedTime <= brickData->animationTime)
	{
		//float x, y = 0.0f;
		//pd->sprite->getPosition(sprite, &x, &y);
		float newY = Lerp(brickData->originalY, brickData->destY, easeOutElastic(brickData->animationElapsedTime / brickData->animationTime));

		pd->sprite->moveTo(sprite, brickData->originalX, newY);
		float deltaTime = ENGINE_deltaTime();
		brickData->animationElapsedTime = brickData->animationElapsedTime + deltaTime;
	}
}

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
	float x = 60.f + ((gridPos - 1) % 11) * (float)(w + BRICK_WIDTH_SPACE_OFFSET);
	float y = 0 - h * 0.5f;
	pd->sprite->moveTo(brick, x, y);
	pd->sprite->setZIndex(brick, 1000);
	pd->sprite->setTag(brick, BRICK);
	pd->sprite->addSprite(brick);

	// Initialize paddle data
	BrickData* brickData = pd_malloc(sizeof(BrickData));
	brickData->visible = true;
	brickData->dx = 0.f;
	brickData->dy = randomFloat(0.f, 120.f);
	brickData->type = BRICK_translateType(type);
	brickData->hp = (brickData->type == HARDENED) ? BRICK_HP * 2 : BRICK_HP;
	brickData->destX = x;
	brickData->destY = 0.f + (int)floor((gridPos - 1.f) / 11.f) * (float)(h + BRICK_HEIGHT_SPACE_OFFSET);
	brickData->flash = 0;
	brickData->originalX = x;
	brickData->originalY = y;
	brickData->animationElapsedTime = 0.0f;
	brickData->animationTime = randomFloat(1.0f, 1.5f);
	pd->sprite->setUserdata(brick, (void*)brickData);

	// Remove temporary string
	pd_free(brickName);
	return brick;
}

void BRICK_destroy(LCDSprite* sprite)
{
	getPlaydateAPI()->sprite->removeSprite(sprite);
	BrickData* brickData = (BrickData*)getPlaydateAPI()->sprite->getUserdata(sprite);
	pd_free(brickData);
	pd_free(sprite);
}

void BRICK_updateSprite(LCDSprite* sprite)
{
	if (sprite)
	{
		PlaydateAPI* pd = getPlaydateAPI();

		animate(sprite);
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
	PlaydateAPI* pd = getPlaydateAPI();

	if (pd->sprite->getTag(other) == WALL) 
	{
		return kCollisionTypeOverlap;
	}
	else 
	{
		return kCollisionTypeBounce;
	}
}
