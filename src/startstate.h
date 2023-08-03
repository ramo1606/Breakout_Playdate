#ifndef STARTSTATE_H
#define STARTSTATE_H

#include "common.h"

void STARTSTATE_setNextState(EMode mode);
EMode STARTSTATE_getNextState(void);
unsigned int STARTSTATE_init(void);
unsigned int STARTSTATE_update(float deltaTime);
unsigned int STARTSTATE_draw(float deltaTime);
unsigned int STARTSTATE_destroy(void);

#endif
