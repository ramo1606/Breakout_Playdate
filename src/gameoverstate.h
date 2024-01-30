#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include "common.h"

unsigned int GAMEOVERSTATE_init(void);
unsigned int GAMEOVERSTATE_update(float deltaTime);
unsigned int GAMEOVERSTATE_draw(float deltaTime);
unsigned int GAMEOVERSTATE_destroy(void);

#endif