#ifndef ENGINE_H
#define ENGINE_H

#include "pd_api.h"

void ENGINE_create(PlaydateAPI* p);
int ENGINE_update(void);
float ENGINE_deltaTime(void);

#endif
