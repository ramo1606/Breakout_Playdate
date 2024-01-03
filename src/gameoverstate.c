#include "gameoverstate.h"

#include "pd_api.h"

#include "transitionmanager.h"
#include "resourcemanager.h"
#include "particles.h"
#include "utils.h"
#include "memory.h"
#include "common.h"

#include <stdbool.h>

typedef struct
{
    EMode nextState;
} GameOverState;

static GameOverState* state = NULL;

void GAMEOVERSTATE_setNextState(EMode mode)
{
	state->nextState = mode;
}

EMode GAMEOVERSTATE_getNextState(void)
{
	return state->nextState;
}

unsigned int GAMEOVERSTATE_init(void)
{
    PlaydateAPI* pd = getPlaydateAPI();

    // Create GameState
    state = pd_malloc(sizeof(GameOverState));

    state->nextState = START;

    return 0;
}

void GAMEOVERSTATE_processInput(void)
{
    PlaydateAPI* pd = getPlaydateAPI();

    PDButtons current;
    pd->system->getButtonState(NULL, NULL, &current);
    bool buttonPressed = false;

    if (current & kButtonA && !buttonPressed)
    {
        //Restart level
    }

    if (current & kButtonB && !buttonPressed)
    {
        //Go To start
    }
}

unsigned int GAMEOVERSTATE_update(float deltaTime)
{
    PlaydateAPI* pd = getPlaydateAPI();

    GAMEOVERSTATE_processInput();


	return 0;
}

unsigned int GAMEOVERSTATE_draw(float deltaTime)
{
    PlaydateAPI* pd = getPlaydateAPI();
    pd->graphics->setBackgroundColor(kColorClear);

    // Draw Logo
    int textWidth = pd->graphics->getTextWidth(font, "GAME OVER STATE", 17, kASCIIEncoding, pd->graphics->getTextTracking());
    pd->graphics->drawText("GAME OVER STATE", 17, kASCIIEncoding, 200 - (textWidth * 0.5f), 160);

	return 0;
}

unsigned int GAMEOVERSTATE_destroy(void)
{
    pd_free(state);
    state = NULL;

	return 0;
}
