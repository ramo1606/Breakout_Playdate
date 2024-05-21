#ifndef PILL_H
#define PILL_H

#include "common.h"

typedef struct PillData
{
	EPillType type;
} PillData;

LCDSprite* PILL_create(float x, float y);
void PILL_destroy(LCDSprite* sprite);
void PILL_updateSprite(LCDSprite* sprite);

EPillType PILL_getType(LCDSprite* sprite);
char PILL_translateType(EPillType type);

void PILL_spawnPufft(float x, float y);

SpriteCollisionResponseType PILL_collisionResponse(LCDSprite* sprite, LCDSprite* other);

#endif