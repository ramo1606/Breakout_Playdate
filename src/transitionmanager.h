#ifndef TRANSITION_MANAGER_H
#define TRANSITION_MANAGER_H
#include "state.h"
#include "common.h"

typedef struct TransitionManager TransitionManager;

void TRANSITION_MANAGER_fadeOut(EMode nextState);
void TRANSITION_MANAGER_fadeIn(void);
unsigned int TRANSITION_MANAGER_init(void);
unsigned int TRANSITION_MANAGER_update(State* state);
unsigned int TRANSITION_MANAGER_draw();
unsigned int TRANSITION_MANAGER_destroy(void);

ETransitionState TRANSITION_MANAGER_transitState();

#endif
