#include "logostate.h"

#include "pd_api.h"

#include "resourcemanager.h"
#include "memory.h"
#include "utils.h"

#include <stdbool.h>

typedef struct
{
    EMode nextState;
} LogoState;

static LogoState* state = NULL;

void LOGOSTATE_processInput(void)
{
    PlaydateAPI* pd = getPlaydateAPI();

	PDButtons current;
	pd->system->getButtonState(NULL, NULL, &current);
	bool buttonPressed = false;

	if (current & kButtonA && !buttonPressed) 
	{
		state->nextState = START;
        buttonPressed = true;
	}
}

EMode LOGOSTATE_getNextState(void)
{
    return state->nextState;
}

unsigned int LOGOSTATE_init(void)
{
    // Create logoState
    state = pd_malloc(sizeof(LogoState));
    state->nextState = LOGO;
    
    return 0;
}

unsigned int LOGOSTATE_update(float deltaTime)
{
    LOGOSTATE_processInput();
    transitionUpdate();
    return 0;
}

unsigned int LOGOSTATE_draw(float deltaTime)
{
    transitionDraw();
    int x = (400 - TEXT_WIDTH) / 2;
	int y = (240 - TEXT_HEIGHT) / 2;
	getPlaydateAPI()->graphics->setFont(font);
	getPlaydateAPI()->graphics->drawText("LOGO!", strlen("LOGO!"), kASCIIEncoding, x, y);

    return 0;
}

unsigned int LOGOSTATE_destroy(void)
{
    pd_free(state);
    state = NULL;
    return 0;
}