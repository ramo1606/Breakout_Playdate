#include "pill.h"
#include "common.h"

struct PillData
{
	EPillType type;

};

LCDSprite* PILL_create(float x, float y, char type)
{
	PlaydateAPI* pd = getPlaydateAPI();

	LCDSprite* pill = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(pill, PILL_updateSprite);

	// Set bitmap
	char* pillName = NULL; // temporary string
	getPlaydateAPI()->system->formatString(&pillName, "pill_%c", type);
	pd->sprite->setImage(pill, RESOURCEMANAGER_getImage(pillName), kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(RESOURCEMANAGER_getImage(pillName), &w, &h, NULL, NULL, NULL);

	// Create bounds rect for ball
	PDRect bounds = PDRectMake(0.f, 0.f, (float)w, (float)h);
	pd->sprite->setBounds(pill, bounds);

	// Create collision rect for ball and set collisions to bounce
	PDRect cr = PDRectMake(1.f, 1.f, (float)(w - 2), (float)(h - 2));
	pd->sprite->setCollideRect(pill, cr);
	pd->sprite->setCollisionResponseFunction(pill, PILL_collisionResponse);

	pd->sprite->moveTo(pill, x, y);
	pd->sprite->setZIndex(pill, 1000);
	pd->sprite->addSprite(pill);

	pd->sprite->setTag(pill, PILL);

	// Initialize paddle data
	PillData* pillData = pd_malloc(sizeof(PillData));
	pillData->type = PILL_translateType(type);
	pd->sprite->setUserdata(pill, (void*)pillData);
	return pill;
}

void PILL_destroy(LCDSprite* sprite)
{
}

void PILL_updateSprite(LCDSprite* sprite)
{
}

EPillType PILL_translateType(char type)
{
	EPillType returnType = SLOW_DOWN;
	if (type == 'a')
		returnType = SLOW_DOWN;
	else if (type == 'b')
		returnType = EXTRA_LIFE;
	else if (type == 'c')
		returnType = STICKY;
	else if (type == 'd')
		returnType = EXPAND;
	else if (type == 'e')
		returnType = REDUCE;
	else if (type == 'f')
		returnType = MEGABALL;
	else if (type == 'g')
		returnType = MULTIBALL;

	return returnType;
}

SpriteCollisionResponseType PILL_collisionResponse(LCDSprite* sprite, LCDSprite* other)
{
	return SpriteCollisionResponseType();
}
