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

typedef struct
{
    uint64_t gameOverCountdown;
    uint64_t blinkSpeed;
    bool gameOverRestart;

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
    state->gameOverCountdown = -1;
    state->blinkSpeed = 0;
    state->gameOverRestart = false;

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
        state->gameOverCountdown = 80;
        state->blinkSpeed = 1;
        state->gameOverRestart = true;
    }

    if (current & kButtonB && !buttonPressed)
    {
        //Go To start
        state->gameOverCountdown = 80;
        state->blinkSpeed = 1;
        state->gameOverRestart = false;
    }
}

unsigned int GAMEOVERSTATE_update(float deltaTime)
{
    PlaydateAPI* pd = getPlaydateAPI();

    float ang = randomFloat(0.0f, PI * 2.f);
    float dx = sinf(ang) * randomFloat(0.0f, 0.6f);
    float dy = cosf(ang) * randomFloat(0.0f, 0.6f);
    LCDColor colors[4] = { ditheringPatterns[2], ditheringPatterns[2], ditheringPatterns[4], ditheringPatterns[8] };
    int topRow = 120;
    int btnRow = 162;
    PARTICLES_addParticle(randomInt(0, 240), topRow, dx, dy, GRAVITY_SMOKE, 70.0f + randomFloat(0.0f, 15.0f), colors, 4, 6.0f + randomFloat(0.0f, 12.0f));
    PARTICLES_addParticle(randomInt(0, 240), btnRow, dx, dy, GRAVITY_SMOKE, 70.0f + randomFloat(0.0f, 15.0f), colors, 4, 6.0f + randomFloat(0.0f, 12.0f));

    if (state->gameOverCountdown < 0)
    {
        GAMEOVERSTATE_processInput();
    }
    else 
    {
        state->gameOverCountdown--;
        //fade
        if (state->gameOverCountdown <= 0) 
        {
            if (state->gameOverRestart) 
            {
                state->gameOverCountdown = -1;
                state->blinkSpeed = 8;
                PARTICLES_removeAllParticles();
                //restart level
            }
            else 
            {
                state->gameOverCountdown = -1;
                state->blinkSpeed = 8;
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
    pd_free(state);
    state = NULL;

	return 0;
}
