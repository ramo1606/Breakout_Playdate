#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include "common.h"

void GAMEOVERSTATE_setNextState(EMode mode);
EMode GAMEOVERSTATE_getNextState(void);
unsigned int GAMEOVERSTATE_init(void);
unsigned int GAMEOVERSTATE_update(float deltaTime);
unsigned int GAMEOVERSTATE_draw(float deltaTime);
unsigned int GAMEOVERSTATE_destroy(void);

#endif