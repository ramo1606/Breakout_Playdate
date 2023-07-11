#pragma once

// Game
#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

#define MAX_BALLS 3

// Paddle
static const float PADDLE_DX = 6.f;

// Ball
static const float BALL_DX = 1.f;
static const float BALL_DY = 1.f;

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
	BLOCK = 2,
} SpriteType;