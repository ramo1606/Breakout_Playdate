#include "startstate.h"
#include "common.h"

#include "transitionmanager.h"
#include "resourcemanager.h"
#include "memory.h"
#include "utils.h"
#include "particles.h"

uint64_t particlesTimer;
uint64_t particleRow;

int startCountdown;

void spawnBackgroundParticles(bool top, uint64_t timer)
{
    float x = 0.0f;
    float y = 0.0f;

    if (timer % 30 == 0)
    {
        if (particleRow == 0)
        {
            particleRow = 1;
        }
        else 
        {
            particleRow = 0;
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

            if ((i + particleRow) % 2 == 0)
            {
                LCDColor colors[1] = { kColorBlack };
                PARTICLES_addParticle(i * 40.0f, y, 0.0f, 0.4f, STATIC_PIX, 10000.0f, colors, 1, 0);
            }
            else 
            {
                //TODO Add random particle sprite
                LCDColor colors[1] = { kColorBlack };
                PARTICLES_addParticle((i * 40.0f) - 20.0f, y - 20.0f, 0.0f, 0.4f, BLUE_ROTATING_SPRITE, 10000.0f, colors, 1, 0);
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
            LCDColor colors[1] = { kColorBlack };
            PARTICLES_addParticle(20 + (i * 40.0f), y, 0.0f, 0.8f, STATIC_PIX, 10000.0f, colors, 1, 0);
        }
    }
}

void startBackgroundParticles(void)
{
    for (size_t i = 0; i < 500; ++i)
    {
        spawnBackgroundParticles(false, i);
    }
}

void STARTSTATE_processInput(void)
{
	PDButtons current;
	pd->system->getButtonState(NULL, NULL, &current);
	bool buttonPressed = false;

	if (current & kButtonA && !buttonPressed) 
	{
        startCountdown = 80;
        //blinkSpeed = 1;

        buttonPressed = true;
	}

    if (((current & kButtonDown) || (current & kButtonUp)) && !buttonPressed)
    {
        //Fast Mode
    }

    if (current & kButtonRight && !buttonPressed)
    {
        // Show HighScore
    }

    if (current & kButtonLeft && !buttonPressed)
    {
        // Hide HighScore
    }
}

unsigned int STARTSTATE_init(void)
{   
    // Init Particles
    particlesTimer = 0;
    particleRow = 0;

    // Init countdown
    startCountdown = -1;

    // Start particles in the background
    startBackgroundParticles();

    return 0;
}

unsigned int STARTSTATE_update(float deltaTime)
{
    // Raining particles
    particlesTimer += 1;
    spawnBackgroundParticles(true, particlesTimer);

    // HighScore

    if (startCountdown < 0)
    {
        // Inputs
        STARTSTATE_processInput();
    }
    else 
    {
        startCountdown -= 1;

        if (startCountdown <= 0) 
        {
            startCountdown = -1;
            //blinkspeed = 8;

            TRANSITION_MANAGER_fadeIn(GAME);
        }
    }

    return 0;
}

unsigned int STARTSTATE_draw(float deltaTime)
{
    pd->graphics->setBackgroundColor(kColorClear);

    // Draw Logo
    int textWidth = pd->graphics->getTextWidth(font, "Press A to start", 17, kASCIIEncoding, pd->graphics->getTextTracking());
    pd->graphics->drawText("Press A to start", 17, kASCIIEncoding, 200 - (textWidth * 0.5f), 160);

    return 0;
}

unsigned int STARTSTATE_destroy(void)
{
    PARTICLES_removeAllParticles();
    return 0;
}
