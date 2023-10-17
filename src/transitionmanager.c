#include "transitionmanager.h"
#include "memory.h"
#include "patterns.h"

#include "pd_api.h"

#include <stdbool.h>

struct TransitionManager
{
    ETransitionState transitState;
    LCDBitmap* fadeBitmap;
    int counter;
    bool changeReady;
    int ditheringIndex;
    EMode selectedNextMenu;
};

static TransitionManager* transitionManager = NULL;

void TRANSITION_MANAGER_fadeIn(EMode nextState)
{
    if (transitionManager->transitState == REST)
    {
        transitionManager->transitState = FADEIN;
        transitionManager->counter = 0;
        transitionManager->selectedNextMenu = nextState;
    }
}

void TRANSITION_MANAGER_fadeOut(void)
{
    transitionManager->changeReady = false;
    transitionManager->transitState = FADEOUT;
    transitionManager->counter = 0;
}

unsigned int TRANSITION_MANAGER_init(void)
{
    PlaydateAPI* pd = getPlaydateAPI();

    transitionManager = pd_malloc(sizeof(TransitionManager));
    transitionManager->transitState = REST;
    transitionManager->counter = 0;
    transitionManager->changeReady = false;
    transitionManager->ditheringIndex = 16;
    transitionManager->fadeBitmap = pd->graphics->newBitmap(400, 240, ditheringPatterns[transitionManager->ditheringIndex]);
    transitionManager->selectedNextMenu = BLANK;
}

unsigned int TRANSITION_MANAGER_update(State* state)
{
    PlaydateAPI* pd = getPlaydateAPI();

    if (transitionManager->changeReady)
    {
        state->setNextState(transitionManager->selectedNextMenu);
        TRANSITION_MANAGER_fadeOut();
    }

    if (transitionManager->transitState == FADEIN)
    {
        if ((transitionManager->counter % 2) == 0)
        {
            pd->graphics->clearBitmap(transitionManager->fadeBitmap, ditheringPatterns[transitionManager->ditheringIndex]);
            
            if (transitionManager->ditheringIndex != 0)
            {
                transitionManager->ditheringIndex--;
            }
            else
            {
                transitionManager->changeReady = true;
            }
        }
    }
    else if (transitionManager->transitState == FADEOUT)
    {
        if ((transitionManager->counter % 4) == 0)
        {
            pd->graphics->clearBitmap(transitionManager->fadeBitmap, ditheringPatterns[transitionManager->ditheringIndex]);
            
            if (transitionManager->ditheringIndex != 16)
            {
                transitionManager->ditheringIndex++;
            }
            else
            {
                transitionManager->transitState = REST;
                transitionManager->selectedNextMenu = BLANK;
            }
        }
    }

    transitionManager->counter++;
}

unsigned int TRANSITION_MANAGER_draw()
{
    PlaydateAPI* pd = getPlaydateAPI();
    
    pd->graphics->drawBitmap(transitionManager->fadeBitmap, 0, 0, kBitmapUnflipped);
}

unsigned int TRANSITION_MANAGER_destroy(void)
{
    pd_free(transitionManager);
}

ETransitionState TRANSITION_MANAGER_transitState()
{
    return transitionManager->transitState;
}
