#ifndef STATEMANAGER_H
#define STATEMANAGER_H
#include "state.h"

typedef struct StateManager StateManager;

StateManager* STATEMANAGER_create(void);
int STATEMANAGER_init(StateManager *statemanager);
int STATEMANAGER_free(StateManager *statemanager);
int STATEMANAGER_push(StateManager *statemanager, State *state);
int STATEMANAGER_pop(StateManager *statemanager);
State *STATEMANAGER_top(StateManager *statemanager);

int STATEMANAGER_update(StateManager *statemanager, float deltatime);
int STATEMANAGER_draw(StateManager *statemanager, float deltatime);

#endif
