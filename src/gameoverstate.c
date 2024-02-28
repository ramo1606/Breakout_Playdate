#include "gameoverstate.h"

#include "common.h"
#include "transitionmanager.h"
#include "resourcemanager.h"
#include "particles.h"
#include "utils.h"
#include "memory.h"
#include "utils.h"
#include "raymath.h"

#include <stdbool.h>

uint64_t gameOverCountdown;
bool gameOverRestart;

unsigned int GAMEOVERSTATE_init(void)
{
    gameOverCountdown = -1;
    gameOverRestart = false;
    //blinkSpeed = 0;

    return 0;
}

void GAMEOVERSTATE_processInput(void)
{
    PDButtons current;
    pd->system->getButtonState(NULL, NULL, &current);
    bool buttonPressed = false;

    if (current & kButtonA && !buttonPressed)
    {
        //Restart level
        gameOverCountdown = 80;
        gameOverRestart = true;
        //blinkSpeed = 1;
    }

    if (current & kButtonB && !buttonPressed)
    {
        //Go To start
        gameOverCountdown = 80;
        gameOverRestart = false;
        //blinkSpeed = 1;
    }
}

unsigned int GAMEOVERSTATE_update(float deltaTime)
{
    float ang = randomFloat(0.0f, PI * 2.f);
    float dx = sinf(ang) * randomFloat(0.0f, 0.6f);
    float dy = cosf(ang) * randomFloat(0.0f, 0.6f);
    LCDColor colors[4] = { ditheringPatterns[2], ditheringPatterns[2], ditheringPatterns[4], ditheringPatterns[8] };
    int topRow = 120;
    int btnRow = 162;
    PARTICLES_addParticle(randomInt(0, 240), topRow, dx, dy, GRAVITY_SMOKE, 70.0f + randomFloat(0.0f, 15.0f), colors, 4, 6.0f + randomFloat(0.0f, 12.0f));
    PARTICLES_addParticle(randomInt(0, 240), btnRow, dx, dy, GRAVITY_SMOKE, 70.0f + randomFloat(0.0f, 15.0f), colors, 4, 6.0f + randomFloat(0.0f, 12.0f));

    if (gameOverCountdown < 0)
    {
        GAMEOVERSTATE_processInput();
    }
    else 
    {
        gameOverCountdown--;
        //fade
        if (gameOverCountdown <= 0) 
        {
            if (gameOverRestart) 
            {
                gameOverCountdown = -1;
                //blinkSpeed = 8;
                PARTICLES_removeAllParticles();
                //restart level
            }
            else 
            {
                gameOverCountdown = -1;
                //blinkSpeed = 8;
                //go to start
                //hs_x = 128
                //hs_dx = 128
                //music()
            }
        }
    }
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
	return 0;
}
