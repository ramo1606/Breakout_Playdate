#include "engine.h"

#include "resourcemanager.h"
#include "memory.h"

#include "raymath.h"
#include "utils.h"
#include "transitionmanager.h"
#include "statemanager.h"
#include "logostate.h"
#include "startstate.h"
#include "gamestate.h"
#include "gameoverstate.h"
#include "leveloverstate.h"
#include "winnerstate.h"
#include "cameraShake.h"
#include "particles.h"

#include <stdlib.h>
#include <math.h>

#define RAYMATH_IMPLEMENTATION

static float deltaTime;
PlaydateAPI* pd = NULL;
EMode mode = BLANK;
bool fastMode = false;
int blinkSpeed = 1;

int ENGINE_create(PlaydateAPI* p)
{
	// Init global variables
	pd = p;					// Set Playdate API to access globally
	mode = LOGO;
	deltaTime = 0.0f;

	// Init subsystems and resources
	RESOURCEMANAGER_load();						// Load resources
	PARTICLES_init();
	UTILS_init();
	TRANSITION_MANAGER_init();

	// Load Fonts
	const char* err;
	font = pd->graphics->loadFont(fontpath, &err);
	if (font == NULL)
		pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

	return 0;
}

int updateState(void)
{
	//State* currentState = STATEMANAGER_top(engine.stateManager);
	switch (mode)
	{
	case LOGO:
		LOGOSTATE_update(deltaTime);
		break;
	case START:
		STARTSTATE_update(deltaTime);
		break;
	case GAME:
		GAMESTATE_update(deltaTime);
		break;
	case GAME_OVER:
		GAMEOVERSTATE_update(deltaTime);
		break;
	case LEVEL_OVER:
		LEVELOVERSTATE_update(deltaTime);
		break;
	case WINNER:
		WINNERSTATE_update(deltaTime);
		break;
	default:
		break;
	}

	return 0;
}

int ENGINE_draw(void);

int ENGINE_update(void)
{
	//BLINK_update();
	SHAKE_update();
	PARTICLES_update();
	//SASH_update();
	updateState();
	TRANSITION_MANAGER_update(deltaTime);
	ENGINE_draw();
	
	// Calculate deltaTime
	deltaTime = pd->system->getElapsedTime();
	pd->system->resetElapsedTime();

	return 0;
}

int drawState(void) 
{
	switch (mode)
	{
	case LOGO:
		LOGOSTATE_draw(deltaTime);
		break;
	case START:
		STARTSTATE_draw(deltaTime);
		break;
	case GAME:
		GAMESTATE_draw(deltaTime);
		break;
	case GAME_OVER:
		GAMEOVERSTATE_draw(deltaTime);
		break;
	case LEVEL_OVER:
		LEVELOVERSTATE_draw(deltaTime);
		break;
	case WINNER:
		WINNERSTATE_draw(deltaTime);
		break;
	default:
		break;
	}

	return 0;
}

int ENGINE_draw(void)
{
	pd->graphics->clear(kColorWhite);
	TRANSITION_MANAGER_draw(deltaTime);
	drawState();
	PARTICLES_draw();
	
	return 0;
}

float ENGINE_deltaTime(void)
{
	return deltaTime;
}
