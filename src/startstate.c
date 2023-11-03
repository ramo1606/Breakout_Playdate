#include "startstate.h"

#include "pd_api.h"

#include "transitionmanager.h"
#include "resourcemanager.h"
#include "particles.h"
#include "utils.h"
#include "memory.h"

#include <stdbool.h>

typedef struct
{
    uint64_t particlesTimer;
    uint64_t particleRow;
    EMode nextState;
} StartState;

static StartState* state = NULL;

void spawnBackgroundParticles(bool top, uint64_t timer);

void startBackgroundParticles() 
{
    for (size_t i = 0; i < 500; ++i) 
    {
        spawnBackgroundParticles(false, i);
    }
}

void spawnBackgroundParticles(bool top, uint64_t timer)
{
    float x = 0.0f;
    float y = 0.0f;

    if (timer % 30 == 0)
    {
        if (state->particleRow == 0)
        {
            state->particleRow = 1;
        }
        else 
        {
            state->particleRow = 0;
        }

        for (size_t i = 1; i < 10; ++i) 
        {
            if (top) 
            {
                y = -10.0f;
            }
            else 
            {
                y = -10.0f + 0.5f * timer;
            }

            if ((i + state->particleRow) % 2 == 0)
            {
                PARTICLES_addParticle(i * 40.0f, y, 0.0f, 0.4f, STATIC_PIX, 10000.0f, kColorBlack, 0);
            }
            else 
            {
                //TODO Add random particle sprite
                PARTICLES_addParticle((i * 40.0f) - 20.0f, y - 20.0f, 0.0f, 0.4f, BLUE_ROTATING_SPRITE, 10000.0f, kColorBlack, 0);
            }
        }
    }

    if (timer % 15 == 0)
    {
        if (top)
        {
            y = -10.0f;
        }
        else
        {
            y = -10.0f + 0.8f * timer;
        }

        for (size_t i = 0; i < 10; ++i)
        {
            PARTICLES_addParticle(20 + (i * 40.0f), y, 0.0f, 0.8f, STATIC_PIX, 10000.0f, kColorBlack, 0);
        }
    }
}

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
        TRANSITION_MANAGER_fadeIn(GAME);
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
    state->particlesTimer = 0;
    state->particleRow = 0;

    startBackgroundParticles();

    return 0;
}

unsigned int STARTSTATE_update(float deltaTime)
{
    PlaydateAPI* pd = getPlaydateAPI();

    // Raining particles
    state->particlesTimer += 1;

    spawnBackgroundParticles(true, state->particlesTimer);

    STARTSTATE_processInput();

    PARTICLES_update();

    return 0;
}

unsigned int STARTSTATE_draw(float deltaTime)
{
    PlaydateAPI* pd = getPlaydateAPI();
    pd->graphics->setBackgroundColor(kColorClear);

    PARTICLES_draw();
    //pd->sprite->updateAndDrawSprites();


    //int x = (400 - TEXT_WIDTH) / 2;
	//int y = (240 - TEXT_HEIGHT) / 2;
	//getPlaydateAPI()->graphics->setFont(font);
	//getPlaydateAPI()->graphics->drawText("START!", strlen("START!"), kASCIIEncoding, x, y);

    return 0;
}

unsigned int STARTSTATE_destroy(void)
{
    PARTICLES_destroy();
    pd_free(state);
    state = NULL;
    return 0;
}
