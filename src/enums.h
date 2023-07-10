#pragma once

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