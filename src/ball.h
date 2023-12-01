#ifndef BALL_H
#define BALL_H

#include "pd_api.h"
#include "common.h"

#include <stdbool.h>

typedef struct BallData
{
	float dx;
	float dy;
	float lastHitDx;
	float lastHitDy;
	float speed;
	float angle;
	bool isStuck;
	bool isDead;
	bool rammed;
	bool isMega;
	int collisionCount;
	int timerSlow;
	int timerMega;
	int timerMegaWait;
	float infiniteCounter;
	ESpriteType lastCollision;
} BallData;

LCDSprite* BALL_create(float x, float y);
void BALL_destroy(LCDSprite* sprite);
void BALL_updateSprite(LCDSprite* sprite);
void BALL_setAngle(LCDSprite* sprite, float angle);

void BALL_spawnTrail(float x, float y, float radius);
void BALL_spawnMegaTrail(float x, float y, float radius);
void BALL_spawnPuft(float x, float y);
void BALL_spawnDeath(float x, float y);

void BALL_resetBall(LCDSprite* sprite);

void BALL_megaballSmash(LCDSprite* sprite);

void BALL_processCollision(LCDSprite* sprite, SpriteCollisionInfo* collision, float x, float y);

SpriteCollisionResponseType BALL_collisionResponse(LCDSprite* sprite, LCDSprite* other);

#endif
