#include "engine.h"

#include "resourcemanager.h"
#include "memory.h"

#include "utils.h"
#include "transitionmanager.h"
#include "statemanager.h"
#include "logostate.h"
#include "startstate.h"
#include "gamestate.h"

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
	//Mode
	EMode mode;
	StateManager* stateManager;
} Engine;

static Engine engine;

void ENGINE_create(PlaydateAPI* p)
{
	setPlaydateAPI(p);							// Set Playdate API to access globally
	MEMORY_initializeMemoryTools(p);			// Initialize memory tools
	RESOURCEMANAGER_load();						// Load resources
	TRANSITION_MANAGER_init();
	PARTICLES_init();
	UTILS_init();

	//Fonts
	const char* err;
	font = getPlaydateAPI()->graphics->loadFont(fontpath, &err);
	if (font == NULL)
		getPlaydateAPI()->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

	engine.stateManager = STATEMANAGER_create();
	STATEMANAGER_init(engine.stateManager);
	
	// Create initial state
	State* logoState = pd_malloc(sizeof(State));
	logoState->setNextState = LOGOSTATE_setNextState;
	logoState->getNextState = LOGOSTATE_getNextState;
	logoState->init = LOGOSTATE_init;
	logoState->update = LOGOSTATE_update;
	logoState->draw = LOGOSTATE_draw;
	logoState->destroy = LOGOSTATE_destroy;

	LOGOSTATE_init();
	STATEMANAGER_push(engine.stateManager, logoState);

	//Mode
	engine.mode = STATEMANAGER_top(engine.stateManager)->getNextState();
}

void updateState(void)
{
	State* currentState = STATEMANAGER_top(engine.stateManager);
	switch (engine.mode)
	{
	case LOGO:
		if (engine.mode != currentState->getNextState())
		{
			engine.mode = currentState->getNextState();
			STATEMANAGER_pop(engine.stateManager);
			pd_free(currentState);

			currentState = pd_malloc(sizeof(State));
			currentState->setNextState = STARTSTATE_setNextState;
			currentState->getNextState = STARTSTATE_getNextState;
			currentState->init = STARTSTATE_init;
			currentState->update = STARTSTATE_update;
			currentState->draw = STARTSTATE_draw;
			currentState->destroy = STARTSTATE_destroy;

			STATEMANAGER_push(engine.stateManager, currentState);
		}
		break;
	case START:
		if (engine.mode != currentState->getNextState())
		{
			engine.mode = currentState->getNextState();

			currentState = pd_malloc(sizeof(State));
			currentState->setNextState = GAMESTATE_setNextState;
			currentState->getNextState = GAMESTATE_getNextState;
			currentState->init = GAMESTATE_init;
			currentState->update = GAMESTATE_update;
			currentState->draw = GAMESTATE_draw;
			currentState->destroy = GAMESTATE_destroy;

			STATEMANAGER_push(engine.stateManager, currentState);
		}
		break;
	case GAME:
		break;
	case GAME_OVER:
		break;
	case GAME_OVER_WAIT:
		break;
	case LEVEL_OVER:
		break;
	case LEVEL_OVER_WAIT:
		break;
	case WINNER:
		break;
	case WINNER_WAIT:
		break;
	default:
		break;
	}
}

int ENGINE_update(void)
{
	updateState();
	getPlaydateAPI()->graphics->clear(kColorWhite);
	
	float deltaTime = getPlaydateAPI()->system->getElapsedTime();
	getPlaydateAPI()->system->resetElapsedTime();

	TRANSITION_MANAGER_update(STATEMANAGER_top(engine.stateManager));
	STATEMANAGER_update(engine.stateManager, deltaTime);

	TRANSITION_MANAGER_draw();
	STATEMANAGER_draw(engine.stateManager, deltaTime);

	return 1;
}
