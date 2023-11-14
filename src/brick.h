#ifndef BRICK_H
#define BRICK_H

#include "pd_api.h"
#include "common.h"

#include <stdbool.h>

typedef struct BrickData BrickData;

LCDSprite* BRICK_create(int gridPos, char type);
void BRICK_destroy(LCDSprite* sprite);
void BRICK_updateSprite(LCDSprite* sprite);

void BRICK_setDx(LCDSprite* sprite, float value);
float BRICK_getDx(LCDSprite* sprite);
void BRICK_setDy(LCDSprite* sprite, float value);
float BRICK_getDy(LCDSprite* sprite);
EBrickType BRICK_getType(LCDSprite* sprite);
void BRICK_setType(LCDSprite* sprite, EBrickType newType);
void BRICK_setFlash(LCDSprite* sprite, int flashTime);
void BRICK_setVisible(LCDSprite* sprite, bool visible);
void BRICK_decreaseHP(LCDSprite* sprite);
int BRICK_getHP(LCDSprite* sprite);


EBrickType BRICK_translateType(char type);

SpriteCollisionResponseType BRICK_collisionResponse(LCDSprite* sprite, LCDSprite* other);

#endif
