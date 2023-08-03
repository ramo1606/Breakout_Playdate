#ifndef LOGOSTATE_H
#define LOGOSTATE_H

#include "common.h"

void LOGOSTATE_setNextState(EMode mode);
EMode LOGOSTATE_getNextState(void);
unsigned int LOGOSTATE_init(void);
unsigned int LOGOSTATE_update(float deltaTime);
unsigned int LOGOSTATE_draw(float deltaTime);
unsigned int LOGOSTATE_destroy(void);

#endif
