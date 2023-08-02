#ifndef BRICK_H
#define BRICK_H

#include "pd_api.h"

typedef struct BrickData BrickData;

LCDSprite* BRICK_create(float x, float y, LCDBitmap* image);
void BRICK_destroy(LCDSprite* sprite);
void BRICK_updateSprite(LCDSprite* sprite);

void BRICK_setDx(LCDSprite* sprite, float value);
float BRICK_getDx(LCDSprite* sprite);
void BRICK_setDy(LCDSprite* sprite, float value);
float BRICK_getDy(LCDSprite* sprite);

SpriteCollisionResponseType BRICK_collisionResponse(LCDSprite* sprite, LCDSprite* other);

#endif
