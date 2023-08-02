#ifndef BRICK_H
#define BRICK_H

#include "pd_api.h"
#include "common.h"

typedef struct BrickData BrickData;

LCDSprite* BRICK_create(float x, float y, LCDBitmap* image, EBrickType type);
void BRICK_destroy(LCDSprite* sprite);
void BRICK_updateSprite(LCDSprite* sprite);

void BRICK_setDx(LCDSprite* sprite, float value);
float BRICK_getDx(LCDSprite* sprite);
void BRICK_setDy(LCDSprite* sprite, float value);
float BRICK_getDy(LCDSprite* sprite);
EBrickType BRICK_getType(LCDSprite* sprite);

EBrickType BRICK_translateType(char type);

SpriteCollisionResponseType BRICK_collisionResponse(LCDSprite* sprite, LCDSprite* other);

#endif
