#ifndef ENGINE_H
#define ENGINE_H

#include "common.h"

int ENGINE_create(PlaydateAPI* p);
int ENGINE_update(void);
int ENGINE_draw(void);
float ENGINE_deltaTime(void);

#endif
