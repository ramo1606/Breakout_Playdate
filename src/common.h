#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pd_api.h"

/*************
* Screen Size
*************/
#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

/***************
* Game constants
****************/
#define MAX_BALLS 3
#define MAX_SICK 12
#define START_LIVES 4

#define INTRO_COUNTDOWN 60
#define START_COUNTDOWN 60
#define GAMEOVER_COUNTDOWN 60

#define HIGH_SCORE_MAX 5;

//Font
static const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
extern LCDFont* font;

/*******
* Enums
********/
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
	WALL,
	PILL,
	UI
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

/*****************
* Global Variables
******************/
extern PlaydateAPI* pd;

// Intro
extern int introCountdown;
extern int startCountdown;

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
extern int suddenDeathBlinkTimer;

extern char* sick[MAX_SICK];

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
