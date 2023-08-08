#include "startstate.h"

#include "pd_api.h"

#include "resourcemanager.h"
#include "memory.h"
#include "patterns.h"

#include <stdbool.h>

typedef struct
{
    EMode nextState;
    LCDBitmap* myBitmap;
    int counter;
    int ditheringIndex;
    bool fadeOut;
} StartState;

static StartState* state = NULL;

void STARTSTATE_setNextState(EMode mode)
{
    state->nextState = mode;
}

void STARTSTATE_processInput(void)
{
    PlaydateAPI* pd = getPlaydateAPI();

	PDButtons current;
	pd->system->getButtonState(NULL, NULL, &current);
	bool buttonPressed = false;

	if (current & kButtonA && !buttonPressed) 
	{
        TRANSITION_MANAGER_startCloseTransit(GAME);
        buttonPressed = true;
	}
}

EMode STARTSTATE_getNextState(void)
{
    return state->nextState;
}

unsigned int STARTSTATE_init(void)
{
    PlaydateAPI* pd = getPlaydateAPI();

    // Create GameState
    state = pd_malloc(sizeof(StartState));
    
    state->nextState = START;

    state->counter = 0;
    state->ditheringIndex = 16;
    state->fadeOut = true;
    state->myBitmap = pd->graphics->newBitmap(400, 240, ditheringPatterns[state->ditheringIndex]);

    return 0;
}

unsigned int STARTSTATE_update(float deltaTime)
{
    PlaydateAPI* pd = getPlaydateAPI();

    STARTSTATE_processInput();

    if ((state->counter % 6) == 0) 
    {
        pd->graphics->clearBitmap(state->myBitmap, ditheringPatterns[state->ditheringIndex]);

        if (state->ditheringIndex == 0 || state->ditheringIndex == 16)
        {
            state->fadeOut = !state->fadeOut;
        }
    
        (state->fadeOut) ? state->ditheringIndex++ : state->ditheringIndex--;
    }
    state->counter++;

    return 0;
}

unsigned int STARTSTATE_draw(float deltaTime)
{
    PlaydateAPI* pd = getPlaydateAPI();

    pd->graphics->drawBitmap(state->myBitmap, 0, 0, kBitmapUnflipped);
    
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
