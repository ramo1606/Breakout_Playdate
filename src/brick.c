#include "brick.h"
#include "memory.h"
#include "rmem.h"
#include "raymath.h"
#include "utils.h"
#include "patterns.h"
#include "cameraShake.h"
#include "particles.h"
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

void checkExplosion(LCDSprite* sprite)
{
	PlaydateAPI* pd = getPlaydateAPI();

	BrickData* brickData = (BrickData*)pd->sprite->getUserdata(sprite);
	if (brickData->type == ZZ && pd->sprite->isVisible(sprite) == true)
	{
		brickData->type = Z;
	}

	if (brickData->type == Z && pd->sprite->isVisible(sprite) == true)
	{
		float x, y = 0.f;
		pd->sprite->getPosition(sprite, &x, &y);
		//explodeBrick();
		BRICK_spawnExplosion(x, y);
		if (getShake() < 0.4f) 
		{
			setShake(getShake() + 0.1f);
		}
	}

	if (brickData->type == ZZ)
	{
		brickData->type = Z;
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

		checkExplosion(sprite);
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

void BRICK_spawnExplosion(float x, float y)
{
	for (size_t i = 0; i < 20; ++i)
	{
		float ang = randomFloat(0.0f, PI * 2.f);
		float dx = sinf(ang) * randomFloat(0.0f, 8.0f);
		float dy = cosf(ang) * randomFloat(0.0f, 8.0f);
		LCDColor colors[5] = { ditheringPatterns[10], ditheringPatterns[10], ditheringPatterns[6], ditheringPatterns[6], ditheringPatterns[4] };
		PARTICLES_addParticle(x, y, dx, dy, SMOKE_BALL, 80.f + randomFloat(0.0f, 15.0f), colors, 5, 3.0f + randomFloat(0.0f, 8.0f));
	}

	for (size_t i = 0; i < 30; ++i)
	{
		float ang = randomFloat(0.0f, PI * 2.f);
		float dx = sinf(ang) * 1.f + randomFloat(0.0f, 6.0f);
		float dy = cosf(ang) * 1.f + randomFloat(0.0f, 6.0f);
		LCDColor colors[5] = { ditheringPatterns[4], ditheringPatterns[1], ditheringPatterns[2], ditheringPatterns[3], ditheringPatterns[8] };
		PARTICLES_addParticle(x, y, dx, dy, SMOKE_BALL, 30.f + randomFloat(0.0f, 15.0f), colors, 5, 4.0f + randomFloat(0.0f, 8.0f));
	}
}

void BRICK_shatterBrick(LCDSprite* sprite, float vx, float vy)
{
	PlaydateAPI* pd = getPlaydateAPI();

	if (getShake() < 0.5f) 
	{
		setShake(getShake() + 0.07f);
	}

	//sfx();

	BrickData* brickData = (BrickData*)pd->sprite->getUserdata(sprite);
	if (brickData) 
	{
		brickData->dx = vx * 1.0f;
		brickData->dy = vy * 1.0f;
	}
	
	int width, height;
	pd->graphics->getBitmapData(pd->sprite->getImage(sprite), &width, &height, NULL, NULL, NULL);

	float posX, posY;
	pd->sprite->getPosition(sprite, &posX, &posY);

	for (int x = 0; x < width; ++x) 
	{
		for (int y = 0; y < height; ++y) 
		{
			if (randomFloat(0.0f, 1.0f) < 0.1f)
			{
				float ang = randomFloat(0.0f, PI * 2.f);
				float dx = sinf(ang) * randomFloat(0.f, 2.f) + vx * 0.5f;
				float dy = cosf(ang) * randomFloat(0.f, 2.f) + vy * 0.5f;

				LCDColor colors[1] = { ditheringPatterns[0] };
				PARTICLES_addParticle(posX + x, posY + y , dx, dy, GRAVITY_PIX, 80.0f, colors, 1, 0);
			}
		}
	}
	
	int chunks = 1 + randomInt(0, 8);
	if (chunks > 0) 
	{
		for (int i = 0; i < chunks; ++i) 
		{
			float ang = randomFloat(0.0f, PI * 2.f);
			float dx = sinf(ang) * randomFloat(0.f, 2.f) + vx * 0.5f;
			float dy = cosf(ang) * randomFloat(0.f, 2.f) + vy * 0.5f;

			//Use sprite instead of
			LCDColor colors[1] = { ditheringPatterns[2] };
			PARTICLES_addParticle(posX, posY, dx, dy, ROTATING_SPRITE, 80.0f, colors, 0, 0);
		}
	}
}

void explodeBrick(LCDSprite* sprite)
{
	PlaydateAPI* pd = getPlaydateAPI();

	BrickData* brickData = (BrickData*)pd->sprite->getUserdata(sprite);

	pd->sprite->setVisible(sprite, false);
	//pd->sprite->

	//bricks[_i].v = false
	//	for j = 1, #bricks do
	//		if j != _i
	//			and bricks[j].v
	//			and abs(bricks[j].x - bricks[_i].x) <= (brick_w + 2)
	//			and abs(bricks[j].y - bricks[_i].y) <= (brick_h + 2)
	//			then
	//			hitbrick(bricks[j], false)
	//			end
	//			end
}