#ifndef PILL_H
#define PILL_H

#include "pd_api.h"

#include <stdbool.h>

typedef struct PillData PillData;

LCDSprite* PILL_create(float x, float y, char type);
void PILL_destroy(LCDSprite* sprite);
void PILL_updateSprite(LCDSprite* sprite);

EPillType PILL_translateType(char type);

SpriteCollisionResponseType PILL_collisionResponse(LCDSprite* sprite, LCDSprite* other);

#endif