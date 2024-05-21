#ifndef GAME_H
#define GAME_H

#include "common.h"

/******
* Main functions
******/
unsigned int GAME_create(PlaydateAPI* p);
unsigned int GAME_update(void);
unsigned int GAME_destroy(void);
float GAME_deltaTime(void);

/*********
* Getters
*********/
LCDSprite* GAME_getPaddle(void);
LCDSprite* GAME_getBall(void);
void GAME_getPowerup(LCDSprite* pill);

void GAME_releaseStuck(void);
void GAME_resetChain();
void GAME_checkSuddenDeath(void);
void GAME_hitBrick(SpriteCollisionInfo* collision, bool combo);
void GAME_explodeBrick(LCDSprite* sprite);

#endif
