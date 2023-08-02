#ifndef PADDLE_H
#define PADDLE_H

#include "pd_api.h"
#include <stdbool.h>

#define PADDLE_INITIAL_X_POS 200
#define PADDLE_INITIAL_Y_POS 230
#define PADDLE_DX 6

typedef struct PaddleData PaddleData;

LCDSprite* PADDLE_create(float x, float y, LCDBitmap* image);
void PADDLE_destroy(LCDSprite* sprite);
void PADDLE_updateSprite(LCDSprite* sprite);

void PADDLE_setDx(LCDSprite* sprite, float value);
float PADDLE_getDx(LCDSprite* sprite);

SpriteCollisionResponseType PADDLE_collisionResponse(LCDSprite* sprite, LCDSprite* other);

#endif
