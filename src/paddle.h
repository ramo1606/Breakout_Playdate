#ifndef PADDLE_H
#define PADDLE_H

#include "common.h"

#define PADDLE_INITIAL_X_POS 200
#define PADDLE_INITIAL_Y_POS 230
#define PADDLE_DX 6

typedef struct PaddleData
{
	float dx;
	int timerReduce;
	int timerExpand;
	EPaddleSize currentSize;

	int speedWind;
} PaddleData;

LCDSprite* PADDLE_create(float x, float y);
void PADDLE_destroy(LCDSprite* sprite);
void PADDLE_updateSprite(LCDSprite* sprite);

void PADDLE_spawnSpeedLines(LCDSprite* sprite, float x, float y);

void PADDLE_reset(LCDSprite* sprite);

SpriteCollisionResponseType PADDLE_collisionResponse(LCDSprite* sprite, LCDSprite* other);

#endif
