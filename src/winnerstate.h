#ifndef WINNERSTATE_H
#define WINNERSTATE_H

#include "common.h"

unsigned int WINNERSTATE_init(void);
unsigned int WINNERSTATE_update(float deltaTime);
unsigned int WINNERSTATE_draw(float deltaTime);
unsigned int WINNERSTATE_destroy(void);

#endif