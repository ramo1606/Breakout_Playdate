#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "common.h"

void GAMESTATE_setNextState(EMode mode);
EMode GAMESTATE_getNextState(void);
unsigned int GAMESTATE_init(void);
unsigned int GAMESTATE_update(float deltaTime);
unsigned int GAMESTATE_draw(float deltaTime);
unsigned int GAMESTATE_destroy(void);

LCDSprite* GAMESTATE_getPaddle(void);
LCDSprite* GAMESTATE_getBall(void);

#endif
