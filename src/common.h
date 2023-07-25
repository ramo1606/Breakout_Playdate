#pragma once
#include "pd_api.h"

// Game
#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

#define MAX_BALLS 3

// Paddle
static const float PADDLE_DX = 6.f;

// Ball
static const float BALL_DX = 1.f;
static const float BALL_DY = 1.f;
static const float BALL_SPEED = 2.5f;
static const float BALL_ANGLE = 1.f;

// Bricks
static const float BRICK_HP = 1.f;

//Font
static const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
static LCDFont* font = NULL;

// Enums
typedef enum
{
	LOGO,
	START,
	GAME,
	GAME_OVER,
	GAME_OVER_WAIT,
	LEVEL_OVER,
	LEVEL_OVER_WAIT,
	WINNER,
	WINNER_WAIT
}EMode;

typedef enum
{
	PADDLE = 0,
	BALL = 1,
	BRICK = 2,
	WALL = 3
} SpriteType;

typedef enum
{
	EMPTY,
	REGULAR,
	INVINCIBLE,
	HARDENED,
	SPLODING,
	POWER
}BrickType;

void set_playdate_API(PlaydateAPI* playdate);
PlaydateAPI* get_playdate_API();
