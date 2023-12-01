#include "pill.h"
#include "utils.h"
#include "memory.h"
#include "resourcemanager.h"
#include "gamestate.h"
#include "raymath.h"
#include "patterns.h"
#include "particles.h"

static const EPillType PillTypes[] = {
	SLOW_DOWN,
	EXTRA_LIFE,
	STICKY,
	EXPAND,
	REDUCE,
	MEGABALL,
	MULTIBALL
};

LCDSprite* PILL_create(float x, float y)
{
	PlaydateAPI* pd = getPlaydateAPI();

	LCDSprite* pill = pd->sprite->newSprite();

	// Initialize paddle data
	PillData* pillData = pd_malloc(sizeof(PillData));
	int typeIndex = randomInt(0, PILLS_NUMBER - 1);
	pillData->type = PillTypes[typeIndex];
	pd->sprite->setUserdata(pill, (void*)pillData);

	// Set bitmap
	char* pillName = NULL; // temporary string
	getPlaydateAPI()->system->formatString(&pillName, "pill_%c", PILL_translateType(pillData->type));
	pd->sprite->setImage(pill, RESOURCEMANAGER_getImage(pillName), kBitmapUnflipped);
	int w, h;
	pd->graphics->getBitmapData(pd->sprite->getImage(pill), &w, &h, NULL, NULL, NULL);

	// Create bounds rect for ball
	PDRect bounds = PDRectMake(0.f, 0.f, (float)w, (float)h);
	pd->sprite->setBounds(pill, bounds);

	pd->sprite->moveTo(pill, x, y);
	pd->sprite->setZIndex(pill, 1000);
	pd->sprite->setUpdateFunction(pill, PILL_updateSprite);
	pd->sprite->addSprite(pill);

	pd->sprite->setTag(pill, PILL);

	// Create collision rect for ball and set collisions to bounce
	PDRect cr = PDRectMake(1.f, 1.f, (float)(w - 2), (float)(h - 2));
	pd->sprite->setCollideRect(pill, cr);
	pd->sprite->setCollisionResponseFunction(pill, PILL_collisionResponse);

	return pill;
}

void PILL_destroy(LCDSprite* sprite)
{
	if (sprite) 
	{
		getPlaydateAPI()->sprite->removeSprite(sprite);
		PillData* pillData = (PillData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		pd_free(pillData);
		pd_free(sprite);
	}
}

void PILL_updateSprite(LCDSprite* sprite)
{
	if (sprite) 
	{
		PlaydateAPI* pd = getPlaydateAPI();
		// Move the ball and check for collisions
		int len = 0;
		float actual_x = 0;
		float actual_y = 0;
		float x, y = 0.0f;
		pd->sprite->getPosition(sprite, &x, &y);
		PDRect pillBounds = pd->sprite->getBounds(sprite);

		SpriteCollisionInfo* collisions = NULL;
		collisions = pd->sprite->moveWithCollisions(sprite, x, y + 1.4f, &actual_x, &actual_y, &len);

		// check if there is any collision
		if (len > 0)
		{
			for (int i = 0; i < len; i++)
			{
				if (pd->sprite->getTag(collisions[i].other) == PADDLE)
				{
					GAMESTATE_getPowerup(collisions[i].sprite);
					PILL_destroy(sprite);
					PILL_spawnPufft(x, y);
					//sfx();
				}
			}
		}

		if (y - pillBounds.height > 240) 
		{
			PILL_destroy(sprite);
		}
	}
}

EPillType PILL_getType(LCDSprite* sprite)
{
	if (sprite)
	{
		PillData* pillData = (PillData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (pillData)
		{
			return pillData->type;
		}
	}
}

char PILL_translateType(EPillType type)
{
	char returnType = 'a';
	if (type == SLOW_DOWN)
		returnType = 'a';
	else if (type == EXTRA_LIFE)
		returnType = 'b';
	else if (type == STICKY)
		returnType = 'c';
	else if (type == EXPAND)
		returnType = 'd';
	else if (type == REDUCE)
		returnType = 'e';
	else if (type == MEGABALL)
		returnType = 'f';
	else if (type == MULTIBALL)
		returnType = 'g';

	return returnType;
}

SpriteCollisionResponseType PILL_collisionResponse(LCDSprite* sprite, LCDSprite* other)
{
	return kCollisionTypeOverlap;
}

void PILL_spawnPufft(float x, float y) 
{
	for (size_t i = 0; i < 20; ++i) 
	{
		float ang = randomFloat(0.0f, PI);
		float sign = ang <= (PI * 0.5f) ? -1.f : 1.f;
		float dx = sinf(ang) * (1 + randomFloat(0.0f, 2.0f)) * sign;
		float dy = cosf(ang) * (1 + randomFloat(0.0f, 2.0f)) * sign;
		LCDColor colors[5] = { ditheringPatterns[0], ditheringPatterns[2], ditheringPatterns[4], ditheringPatterns[6], ditheringPatterns[8] };
		PARTICLES_addParticle(x, y, dx, dy, SMOKE_BALL, 20.f + randomFloat(0.0f, 15.0f), colors, 5, 2.0f + randomFloat(0.0f, 8.0f));
	}
}