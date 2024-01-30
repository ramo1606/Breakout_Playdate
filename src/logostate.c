#include "logostate.h"
#include "common.h"
#include "transitionmanager.h"

#define INTRO_TIME 60

int introTime;


unsigned int LOGOSTATE_init(void)
{
    introTime = 0;

    return 0;
}

unsigned int LOGOSTATE_update(float deltaTime)
{
    introTime += 1;
    if (introTime > INTRO_TIME)
    {
        TRANSITION_MANAGER_fadeIn(START);
        introTime = 0;
    }
    return 0;
}

unsigned int LOGOSTATE_draw(float deltaTime)
{
    //transitionDraw();
    int x = (400 - TEXT_WIDTH) / 2;
	int y = (240 - TEXT_HEIGHT) / 2;
	pd->graphics->setFont(font);
	pd->graphics->drawText("LOGO!", strlen("LOGO!"), kASCIIEncoding, x, y);

    return 0;
}

unsigned int LOGOSTATE_destroy(void)
{
    return 0;
}
