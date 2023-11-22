#ifndef BRICK_H
#define BRICK_H

#include "pd_api.h"
#include "common.h"

#include <stdbool.h>

typedef struct BrickData
{
	bool visible;
	float dx;
	float dy;
	int hp;
	EBrickType type;
	float destX;
	float destY;
	int flash;
	float originalX;
	float originalY;
	float animationElapsedTime;
	float animationTime;
} BrickData;

LCDSprite* BRICK_create(int gridPos, char type);
void BRICK_destroy(LCDSprite* sprite);
void BRICK_updateSprite(LCDSprite* sprite);

EBrickType BRICK_translateType(char type);

SpriteCollisionResponseType BRICK_collisionResponse(LCDSprite* sprite, LCDSprite* other);

#endif
