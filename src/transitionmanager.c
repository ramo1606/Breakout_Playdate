#include "transitionmanager.h"
#include "memory.h"

#include "pd_api.h"

#include <stdbool.h>

#define TRANSITION_TIME_MAX 15
#define TRANSITION_RECT_W_MAX 400

struct TransitionManager
{
    ETransitionState transitState;
    int closeTime;
    int openTime;
    bool changeReady;
    int rectWidth;
    int rectChangeSpd;
    EMode selectedNextMenu;
};

static TransitionManager* transitionManager = NULL;

void TRANSITION_MANAGER_startCloseTransit(EMode nextState)
{
    if (transitionManager->transitState == REST)
    {
        transitionManager->transitState = CLOSE;
        transitionManager->closeTime = TRANSITION_TIME_MAX;
        transitionManager->selectedNextMenu = nextState;
    }
}

void TRANSITION_MANAGER_startOpenTransit(void)
{
    transitionManager->changeReady = false;
    transitionManager->transitState = OPEN;
    transitionManager->rectWidth = 0;
    transitionManager->openTime = TRANSITION_TIME_MAX;
}

unsigned int TRANSITION_MANAGER_init(void)
{
    transitionManager = pd_malloc(sizeof(TransitionManager));
    transitionManager->transitState = REST;
    transitionManager->openTime = 0;
    transitionManager->closeTime = 0;
    transitionManager->changeReady = false;
    transitionManager->rectWidth = 0;
    transitionManager->rectChangeSpd = 30;
    transitionManager->selectedNextMenu = BLANK;
}

unsigned int TRANSITION_MANAGER_update(State* state)
{
    if (transitionManager->changeReady)
    {
        state->setNextState(transitionManager->selectedNextMenu);
        TRANSITION_MANAGER_startOpenTransit();
    }

    if (transitionManager->transitState == CLOSE)
    {
        if (transitionManager->closeTime > 0)
        {
            transitionManager->closeTime -= 1;
            transitionManager->rectWidth += transitionManager->rectChangeSpd;
        }
        else
        {
            transitionManager->changeReady = true;
        }
    }
    else if (transitionManager->transitState == OPEN)
    {
        if (transitionManager->openTime > 0)
        {
            transitionManager->openTime -= 1;
            transitionManager->rectWidth += transitionManager->rectChangeSpd;
        }
        else
        {
            transitionManager->transitState = REST;
            transitionManager->selectedNextMenu = BLANK;
            transitionManager-> rectWidth = 0;
        }
    }
}

unsigned int TRANSITION_MANAGER_draw()
{
    PlaydateAPI* pd = getPlaydateAPI();
    if (transitionManager->transitState == CLOSE)
    {
        pd->graphics->fillRect(0, 0, transitionManager->rectWidth, pd->display->getHeight(), kColorBlack);
    }
    else if (transitionManager->transitState == OPEN)
    {
        pd->graphics->fillRect(transitionManager->rectWidth, 0, pd->display->getWidth(), pd->display->getHeight(), kColorBlack);
    }
    else if (transitionManager->transitState == REST)
    {
    }
}

unsigned int TRANSITION_MANAGER_destroy(void)
{
    pd_free(transitionManager);
}

ETransitionState TRANSITION_MANAGER_transitState()
{
    return transitionManager->transitState;
}
