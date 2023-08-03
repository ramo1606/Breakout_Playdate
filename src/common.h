#ifndef COMMON_H
#define COMMON_H

#include "pd_api.h"

// Game
#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

//Font
static const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
static LCDFont* font = NULL;

// Enums
typedef enum
{
	BLANK,
	LOGO,
	START,
	GAME,
	GAME_OVER,
	GAME_OVER_WAIT,
	LEVEL_OVER,
	LEVEL_OVER_WAIT,
	WINNER,
	WINNER_WAIT
} EMode;

typedef enum
{
	NONE,
	PADDLE,
	BALL,
	BRICK,
	WALL
} ESpriteType;

typedef enum
{
	EMPTY,
	REGULAR,
	INVINCIBLE,
	HARDENED,
	SPLODING,
	POWER
} EBrickType;

typedef enum
{
	REST,
	CLOSE,
	OPEN
} ETransitionState;

// PlaydateAPI
void setPlaydateAPI(PlaydateAPI* playdate);
PlaydateAPI* getPlaydateAPI(void);

#endif
