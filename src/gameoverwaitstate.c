#include "gameoverwaitstate.h"
#include "pd_api.h"

#include "transitionmanager.h"
#include "resourcemanager.h"
#include "particles.h"
#include "utils.h"
#include "memory.h"
#include "common.h"
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

void GAMEOVERWAITSTATE_setNextState(EMode mode)
{
}

EMode GAMEOVERWAITSTATE_getNextState(void)
{
	return BLANK;
}

unsigned int GAMEOVERWAITSTATE_init(void)
{
	return 0;
}

unsigned int GAMEOVERWAITSTATE_update(float deltaTime)
{
	return 0;
}

unsigned int GAMEOVERWAITSTATE_draw(float deltaTime)
{
	return 0;
}

unsigned int GAMEOVERWAITSTATE_destroy(void)
{
	return 0;
}
