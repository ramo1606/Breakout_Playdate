#include "brick.h"

#include "memory.h"
#include "rmem.h"
#include "raymath.h"
#include "utils.h"
#include "patterns.h"
#include "cameraShake.h"
#include "particles.h"
#include "resourcemanager.h"
#include "easing.h"
#include "game.h"

#include <stdbool.h>

#define BRICK_WIDTH_SPACE_OFFSET 3
#define BRICK_HEIGHT_SPACE_OFFSET 2

static const int BRICK_HP = 1;

static void animate(LCDSprite* sprite) 
{
	BrickData* brickData = (BrickData*)pd->sprite->getUserdata(sprite);
	if (brickData) 
	{
		if (brickData->visible)
		{
			if (brickData->animationElapsedTime <= brickData->animationTime)
			{
				float newX = Lerp(brickData->originalX, brickData->destX, easeOutElastic(brickData->animationElapsedTime / brickData->animationTime));
				float newY = Lerp(brickData->originalY, brickData->destY, easeOutElastic(brickData->animationElapsedTime / brickData->animationTime));

				pd->sprite->moveTo(sprite, newX, newY);
				float deltaTime = GAME_deltaTime();
				brickData->animationElapsedTime = brickData->animationElapsedTime + deltaTime;
			}
		}
	}
}

static void checkExplosion(LCDSprite* sprite)
{
	BrickData* brickData = (BrickData*)pd->sprite->getUserdata(sprite);
	if (brickData->type == ZZ && pd->sprite->isVisible(sprite))
	{
		brickData->type = Z;
	}

	if (brickData->type == Z && pd->sprite->isVisible(sprite) == true)
	{
		float x, y = 0.f;
		pd->sprite->getPosition(sprite, &x, &y);
		GAME_explodeBrick(sprite);
		BRICK_spawnExplosion(x, y);
		if (SHAKE_get() < 0.4f) 
		{
			SHAKE_set(SHAKE_get() + 0.1f);
		}
	}

	if (brickData->type == ZZ)
	{
		brickData->type = Z;
	}
}

static void checkFlash(LCDSprite* sprite)
{
	if (sprite) 
	{
		BrickData* brickData = (BrickData*)pd->sprite->getUserdata(sprite);
		if (brickData)
		{
			if (brickData->flash > 0 && brickData->visible)
			{
				pd->sprite->setDrawMode(sprite, kDrawModeFillBlack);
				pd->sprite->setStencilPattern(sprite, ditheringPatterns[8]);
				brickData->flash += 1;
				if (brickData->flash == 10)
				{
					brickData->visible = false;
					pd->sprite->setVisible(sprite, 0);
					pd->sprite->setCollisionsEnabled(sprite, 0);
					brickData->flash = 0;
				}
			}
		}
	}
}

LCDSprite* BRICK_create(int gridPos, char type)
{
	// Create new Sprite
	LCDSprite* brick = pd->sprite->newSprite();

	// Set update function
	pd->sprite->setUpdateFunction(brick, BRICK_updateSprite);

	// Set bitmap
	char* brickName = NULL; // temporary string
	pd->system->formatString(&brickName, "brick_%c", type);
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
	brickData->type = BRICK_translateType(type);
	brickData->hp = (brickData->type == HARDENED) ? BRICK_HP * 2 : BRICK_HP;
	brickData->destX = x;
	brickData->destY = 0.f + (int)floor((gridPos - 1.f) / 11.f) * (float)(h + BRICK_HEIGHT_SPACE_OFFSET);
	brickData->flash = 0;
	brickData->originalX = x;
	brickData->originalY = y;
	brickData->animationElapsedTime = 0.0f;
	brickData->animationTime = randomFloat(0.8f, 1.6f);
	brickData->animating = false;
	pd->sprite->setUserdata(brick, (void*)brickData);

	// Remove temporary string
	pd_free(brickName);
	return brick;
}

void BRICK_destroy(LCDSprite* sprite)
{
	pd->sprite->removeSprite(sprite);
	BrickData* brickData = (BrickData*)pd->sprite->getUserdata(sprite);
	pd_free(brickData);
	pd_free(sprite);
}

void BRICK_updateSprite(LCDSprite* sprite)
{
	if (sprite)
	{
		checkExplosion(sprite);
		animate(sprite);
		checkFlash(sprite);
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
	if (SHAKE_get() < 0.5f) 
	{
		SHAKE_set(SHAKE_get() + 0.07f);
	}

	//sfx();

	BrickData* brickData = (BrickData*)pd->sprite->getUserdata(sprite);
	if (brickData) 
	{
		float x, y;
		pd->sprite->getPosition(sprite, &x, &y);
		brickData->originalX = x;
		brickData->originalY = y;
		brickData->destX = brickData->originalX + (vx * 3.f);
		brickData->destY = brickData->originalY + (vy * 3.f);
		brickData->animationElapsedTime = 0.0f;
		brickData->animationTime = 0.12f;
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
				float dx = sinf(ang) * randomFloat(0.f, 3.f) + vx * 0.5f;
				float dy = cosf(ang) * randomFloat(0.f, 3.f) + vy * 0.5f;

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