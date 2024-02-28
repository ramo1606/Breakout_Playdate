#ifndef COMMON_H
#define COMMON_H

#include "pd_api.h"
#include <stdbool.h>

// Game
#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

//Font
static const char* fontpath = "fonts/R-TypeII";
static LCDFont* font = NULL;

// Enums
typedef enum
{
	BLANK,
	LOGO,
	START,
	GAME,
	GAME_OVER,
	//GAME_OVER_WAIT,
	LEVEL_OVER,
	//LEVEL_OVER_WAIT,
	WINNER,
	//WINNER_WAIT
} EMode;

typedef enum
{
	NONE,
	PADDLE,
	BALL,
	BRICK,
	WALL,
	PILL
} ESpriteType;

typedef enum
{
	EMPTY,
	REGULAR,
	INVINCIBLE,
	HARDENED,
	SPLODING,
	POWER,
	ZZ,
	Z
} EBrickType;

typedef enum
{
	SLOW_DOWN,
	EXTRA_LIFE,
	STICKY,
	EXPAND,
	REDUCE,
	MEGABALL,
	MULTIBALL
} EPillType;

typedef enum
{
	REST,
	FADEOUT,
	FADEIN
} ETransitionState;

typedef enum
{
	SHORT,
	MEDIUM,
	LARGE
} EPaddleSize;

// Global Variables
extern PlaydateAPI* pd;

// Game
extern EMode mode;
extern bool fastMode;

// Effects
extern float fadePercentage;

// Blink
extern int blinkFrame;
extern int blinkSpeed;
extern int blinkBlack; 
extern int blinkBlackIndex;
extern int blinkWhite; 
extern int blinkWhiteIndex;

// Suden Death
extern LCDSprite* suddenDeathBrick;
extern int suddenDeathTimer;
extern int suddenDeathThresh;

extern LCDPattern BayerDither00;
extern LCDPattern BayerDither01;
extern LCDPattern BayerDither02;
extern LCDPattern BayerDither03;
extern LCDPattern BayerDither04;
extern LCDPattern BayerDither05;
extern LCDPattern BayerDither06;
extern LCDPattern BayerDither07;
extern LCDPattern BayerDither08;
extern LCDPattern BayerDither09;
extern LCDPattern BayerDither10;
extern LCDPattern BayerDither11;
extern LCDPattern BayerDither12;
extern LCDPattern BayerDither13;
extern LCDPattern BayerDither14;
extern LCDPattern BayerDither15;
extern LCDPattern BayerDither16;
extern LCDPattern* ditheringPatterns[17];

#endif
