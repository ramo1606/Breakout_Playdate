#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "common.h"

EMode GAMESTATE_getNextState(void);
unsigned int GAMESTATE_init(void);
unsigned int GAMESTATE_update(float deltaTime);
unsigned int GAMESTATE_draw(float deltaTime);
unsigned int GAMESTATE_destroy(void);

#endif
