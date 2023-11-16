#ifndef BALL_H
#define BALL_H

#include "pd_api.h"

#include <stdbool.h>

typedef struct BallData BallData;

LCDSprite* BALL_create(float x, float y);
void BALL_destroy(LCDSprite* sprite);
void BALL_updateSprite(LCDSprite* sprite);

void BALL_setDx(LCDSprite* sprite, float value);
float BALL_getDx(LCDSprite* sprite);
void BALL_setDy(LCDSprite* sprite, float value);
float BALL_getDy(LCDSprite* sprite);
void BALL_setStuck(LCDSprite* sprite, bool stuck);
bool BALL_isStuck(LCDSprite* sprite);
bool BALL_isDead(LCDSprite* sprite);
void BALL_setAngle(LCDSprite* sprite, float angle);
bool BALL_rammed(LCDSprite* sprite);
void BALL_setRammed(LCDSprite* sprite, bool rammed);
void BALL_resetInfiniteCounter(LCDSprite* sprite);
void BALL_increaseInfiniteCounter(LCDSprite* sprite, float value);
float BALL_infiniteCounter(LCDSprite* sprite);
int BALL_getMegaballTimer(LCDSprite* sprite);
float BALL_getLastHitDx(LCDSprite* sprite);
float BALL_getLastHitDy(LCDSprite* sprite);

void BALL_spawnTrail(float x, float y, float radius);
void BALL_spawnPuft(float x, float y);

void BALL_resetBall(LCDSprite* sprite);

void BALL_megaballSmash(LCDSprite* sprite);

void BALL_processCollision(LCDSprite* sprite, SpriteCollisionInfo* collision, float x, float y);

SpriteCollisionResponseType BALL_collisionResponse(LCDSprite* sprite, LCDSprite* other);

#endif
