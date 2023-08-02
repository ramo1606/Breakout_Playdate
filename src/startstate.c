#include "startstate.h"

#include "pd_api.h"

#include "resourcemanager.h"
#include "memory.h"

#include <stdbool.h>

typedef struct
{
    EMode nextState;
} StartState;

static StartState* state = NULL;

void STARTSTATE_processInput(void)
{
    PlaydateAPI* pd = getPlaydateAPI();

	PDButtons current;
	pd->system->getButtonState(NULL, NULL, &current);
	bool buttonPressed = false;

	if (current & kButtonA && !buttonPressed) 
	{
		state->nextState = GAME;
        buttonPressed = true;
	}
}

EMode STARTSTATE_getNextState(void)
{
    return state->nextState;
}

unsigned int STARTSTATE_init(void)
{
    // Create GameState
    state = pd_malloc(sizeof(StartState));
    
    state->nextState = START;
    return 0;
}

unsigned int STARTSTATE_update(float deltaTime)
{
    STARTSTATE_processInput();
    return 0;
}

unsigned int STARTSTATE_draw(float deltaTime)
{
    int x = (400 - TEXT_WIDTH) / 2;
	int y = (240 - TEXT_HEIGHT) / 2;
	getPlaydateAPI()->graphics->setFont(font);
	getPlaydateAPI()->graphics->drawText("START!", strlen("START!"), kASCIIEncoding, x, y);

    return 0;
}

unsigned int STARTSTATE_destroy(void)
{
    pd_free(state);
    state = NULL;
    return 0;
}
