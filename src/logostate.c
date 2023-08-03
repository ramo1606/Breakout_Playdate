#include "logostate.h"

#include "pd_api.h"

#include "transitionmanager.h"
#include "resourcemanager.h"
#include "memory.h"
#include "utils.h"

#include <stdbool.h>

typedef struct
{
    int introTime;
    EMode nextState;
} LogoState;

static LogoState* state = NULL;

void LOGOSTATE_setNextMode(EMode mode)
{
    state->nextState = mode;
}

EMode LOGOSTATE_getNextState(void)
{
    return state->nextState;
}

unsigned int LOGOSTATE_init(void)
{
    // Create logoState
    state = pd_malloc(sizeof(LogoState));
    state->introTime = 0;
    state->nextState = LOGO;
    
    return 0;
}

unsigned int LOGOSTATE_update(float deltaTime)
{
    state->introTime += 1;
    if (state->introTime > 30)
    {
        TRANSITION_MANAGER_startCloseTransit(START);
        state->introTime = 0;
    }
    TRANSITION_MANAGER_update(state);
    return 0;
}

unsigned int LOGOSTATE_draw(float deltaTime)
{
    //transitionDraw();
    int x = (400 - TEXT_WIDTH) / 2;
	int y = (240 - TEXT_HEIGHT) / 2;
	getPlaydateAPI()->graphics->setFont(font);
	getPlaydateAPI()->graphics->drawText("LOGO!", strlen("LOGO!"), kASCIIEncoding, x, y);
    TRANSITION_MANAGER_draw();

    return 0;
}

unsigned int LOGOSTATE_destroy(void)
{
    pd_free(state);
    state = NULL;
    return 0;
}
