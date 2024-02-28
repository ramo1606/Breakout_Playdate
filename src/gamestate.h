#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "common.h"

/****************
* State functions
*****************/
unsigned int GAMESTATE_init(void);
unsigned int GAMESTATE_update(float deltaTime);
unsigned int GAMESTATE_draw(float deltaTime);
unsigned int GAMESTATE_destroy(void);

/*********
* Getters
*********/
LCDSprite* GAMESTATE_getPaddle(void);
LCDSprite* GAMESTATE_getBall(void);
void GAMESTATE_getPowerup(LCDSprite* pill);

void GAMESTATE_resetChain();
void GAMESTATE_checkSD(void);
void GAMESTATE_hitBrick(SpriteCollisionInfo* collision, bool combo);

#endif
