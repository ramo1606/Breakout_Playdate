#ifndef GAMEOVERWAITSTATE_H
#define GAMEOVERWAITSTATE_H

#include "common.h"

unsigned int GAMEOVERWAITSTATE_init(void);
unsigned int GAMEOVERWAITSTATE_update(float deltaTime);
unsigned int GAMEOVERWAITSTATE_draw(float deltaTime);
unsigned int GAMEOVERWAITSTATE_destroy(void);

#endif