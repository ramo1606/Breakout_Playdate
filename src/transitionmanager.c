#include "transitionmanager.h"
#include "common.h"

// States
#include "logostate.h"
#include "startstate.h"
#include "gamestate.h"
#include "gameoverstate.h"
#include "leveloverstate.h"
#include "winnerstate.h"

ETransitionState transitState;
LCDBitmap* fadeBitmap;
int counter;
bool changeReady;
int ditheringIndex;
EMode selectedNextMenu;

void TRANSITION_MANAGER_fadeIn(EMode nextState)
{
    if (transitState == REST)
    {
        transitState = FADEIN;
        counter = 0;
        selectedNextMenu = nextState;
    }
}

void TRANSITION_MANAGER_fadeOut(void)
{
    changeReady = false;
    transitState = FADEOUT;
    counter = 0;
}

unsigned int TRANSITION_MANAGER_init(void)
{
    transitState = REST;
    counter = 0;
    changeReady = false;
    ditheringIndex = 16;
    fadeBitmap = pd->graphics->newBitmap(400, 240, ditheringPatterns[ditheringIndex]);
    selectedNextMenu = BLANK;

    return 0;
}

unsigned int TRANSITION_MANAGER_exitState(void) 
{
    switch (mode)
    {
    case LOGO:
        LOGOSTATE_destroy();
        break;
    case START:
        STARTSTATE_destroy();
        break;
    case GAME:
        GAMESTATE_destroy();
        break;
    case GAME_OVER:
        GAMEOVERSTATE_destroy();
        break;
    case LEVEL_OVER:
        LEVELOVERSTATE_destroy();
        break;
    case WINNER:
        WINNERSTATE_destroy();
        break;
    default:
        break;
    }

    return 0;
}

unsigned int TRANSITION_MANAGER_initState(void)
{
    switch (mode)
    {
    case LOGO:
        LOGOSTATE_init();
        break;
    case START:
        STARTSTATE_init();
        break;
    case GAME:
        GAMESTATE_init();
        break;
    case GAME_OVER:
        GAMEOVERSTATE_init();
        break;
    case LEVEL_OVER:
        LEVELOVERSTATE_init();
        break;
    case WINNER:
        WINNERSTATE_init();
        break;
    default:
        break;
    }

    return 0;
}

unsigned int TRANSITION_MANAGER_update(float deltaTime)
{
    if (changeReady)
    {
        TRANSITION_MANAGER_exitState();
        mode = selectedNextMenu;
        TRANSITION_MANAGER_initState();
        TRANSITION_MANAGER_fadeOut();
    }

    if (transitState == FADEIN)
    {
        if ((counter % 2) == 0)
        {
            pd->graphics->clearBitmap(fadeBitmap, ditheringPatterns[ditheringIndex]);
            
            if (ditheringIndex != 0)
            {
                ditheringIndex--;
            }
            else
            {
                changeReady = true;
            }
        }
    }
    else if (transitState == FADEOUT)
    {
        if ((counter % 4) == 0)
        {
            pd->graphics->clearBitmap(fadeBitmap, ditheringPatterns[ditheringIndex]);
            
            if (ditheringIndex != 16)
            {
                ditheringIndex++;
            }
            else
            {
                transitState = REST;
                selectedNextMenu = BLANK;
            }
        }
    }

    counter++;

    return 0;
}

unsigned int TRANSITION_MANAGER_draw(float deltaTime)
{
    pd->graphics->drawBitmap(fadeBitmap, 0, 0, kBitmapUnflipped);

    return 0;
}

unsigned int TRANSITION_MANAGER_destroy(void)
{
    return 0;
}
