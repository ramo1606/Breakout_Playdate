#include "game.h"

// Third Parties
#include "raymath.h"
#include "DG_dynarr.h"

// Management
#include "resourcemanager.h"
#include "memory.h"
#include "utils.h"

// Effects and particles
#include "cameraShake.h"
#include "particles.h"
#include "blink.h"
#include "sash.h"

// Game Objects
#include "level.h"
#include "paddle.h"
#include "ball.h"
#include "brick.h"
#include "pill.h"
#include "popup.h"

// Standar Libs
#include <stdlib.h>
//#include <math.h>

#define RAYMATH_IMPLEMENTATION

/********
* Structs
*********/
typedef struct
{
	LCDSprite* Top;
	LCDSprite* Bottom;
	LCDSprite* Right;
	LCDSprite* Left;
} Walls;

/**********************
* Global Game Variables
***********************/
PlaydateAPI* pd = NULL;
static float deltaTime;
EMode mode = BLANK;
bool fastMode = false;

/**************
* Game Entities
***************/
DA_TYPEDEF(LCDSprite*, BricksArrayType);
DA_TYPEDEF(LCDSprite*, PillsArrayType);

LCDSprite* paddle = NULL;
LCDSprite* balls[MAX_BALLS];
BricksArrayType bricks;
PillsArrayType pills;
Walls walls;

/****
* UI
****/
LCDSprite* LevelFinishedUI = NULL;

/*************
* Suden Death
**************/
LCDSprite* suddenDeathBrick = NULL;
int suddenDeathTimer = 0;
int suddenDeathThresh = 3;
int suddenDeathBlinkTimer = 0;

/******
* Arrow
*******/
float arrowMultiplier1 = 1;
float arrowMultiplier2 = 1;
int arrowMultiplayerFrame = 0;

/********
* Levels
*********/
int currentLevel = 0;

/*************
* Player Data
**************/
int lives = START_LIVES;
int chain = 1;
int scoreMul = 1;
int points = 0;
int points2 = 0;

// Blink
//int blinkFrame = 0;
//int blinkSpeed = 8;

//int blinkBlack = 7;
//int blinkBlackIndex = 1;
//int blinkWhite = 7;
//int blinkWhiteIndex = 1;

/***********
* High Score
************/
int highScore[] = { 0, 0, 0, 0, 0 };
int highScoreTable[] = { 0, 0, 0, 0, 0 };
int highScore1[] = { 0, 0, 0, 0, 0 };
int highScore2[] = { 0, 0, 0, 0, 0 };
int highScore3[] = { 0, 0, 0, 0, 0 };
bool highScoreBool[] = { true, false, false, false, false };

char highScoreChars[] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };
float highScoreX = 128.f;
float highScoreDX = 128.f;
bool logHighScore = false;

int initials[] = { 1, 1, 1 };
int selectedInitial = 1;
LCDFont* font = NULL;

/*******
* Other
********/
int introCountdown = INTRO_COUNTDOWN;
int startCountdown = -1;
int gameOverCountdown = GAMEOVER_COUNTDOWN;
bool gameOverRestart = false;
int particlesTimer = 0;
int particleRow = 0;

/******
* Sash
******/
char* sick[MAX_SICK] = {
		"so sick!",
		"yeeee boiii!",
		"impressive!",
		"i can't even...",
		"it's lit!",
		"mah dude!",
		"c-c-combo!",
		"winning!",
		"niiiice!",
		"woah!",
		"seriously now?",
		"maximum pwnage!"
};

void restartLevel(void);
void loadLevelBricks(void);
void serveBall(void);
void resetPills(void);
void createWalls(void);
void updateArrow(void);
void drawArrow(void);
void boostChain(void);

/*************
* 
* Game Flow
**************/
void startGame(void)
{
	// Init game data
	currentLevel = 0;
	createWalls();

	// Create Paddle
	paddle = PADDLE_create(PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS);
	PDRect paddleRect = pd->sprite->getCollideRect(paddle);

	// Create Balls
	balls[0] = BALL_create(PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS - (paddleRect.width * 0.5f));
	balls[1] = BALL_create(PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS - (paddleRect.width * 0.5f));
	balls[2] = BALL_create(PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS - (paddleRect.width * 0.5f));

	restartLevel();
}

void winGame(void)
{
	//music();
	mode = WINNER_WAIT;
	gameOverCountdown = 60;
	//blinkSpeed = 16;

	//Manage HighScore
	if (points2 > highScoreTable[4] || (points2 == highScoreTable[4] && points > highScoreTable[4]))
	{
		logHighScore = true;
		selectedInitial = 1;
	}
	else 
	{
		logHighScore = false;
		//resetHSB();
	}
}

void gameOver(void)
{
	//music();
	mode = GAME_OVER_WAIT;
	gameOverCountdown = 60;
	//blinkSpeed = 16;
	//resetHSB();
}

/*************
* Levels Flow
**************/
void restartLevel(void)
{
	mode = GAME;

	// Build Bricks
	loadLevelBricks();

	// Reset Paddle
	pd->sprite->moveTo(paddle, PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS);
	PADDLE_resetPaddle(paddle);

	// Reset Balls
	for (size_t i = 0; i < MAX_BALLS; ++i)
	{
		BALL_resetBall(balls[i]);
	}

	BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[0]);

	// Setup fastMode
	if (fastMode)
	{
		ballData->speed = ballData->speed * 1.5f;
	}

	// Game variables
	lives = START_LIVES;
	points = 0;
	points2 = 0;
	chain = 1;
	char stage[] = "stage 100";
	sprintf(stage, "Stage %i", currentLevel);
	SASH_show(stage, kColorBlack, kColorWhite);

	serveBall();
}

void nextLevel(void)
{
	mode = GAME;
	// Set Next level
	currentLevel += 1;

	if (currentLevel > LEVEL_getAmount())
	{
		winGame();
	}
	else
	{
		// Load Bricks
		loadLevelBricks();

		// Reset chain
		chain = 1;

		// Reset Balls and Paddle
		pd->sprite->moveTo(paddle, PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS);
		PADDLE_resetPaddle(paddle);

		//showsash("stage "..levelnum, 0, 7)

		serveBall();
	}
}

bool levelFinished(void)
{
	bool returnValue = false;

	if (da_count(bricks) == 0)
	{
		return true;
	}

	// checks if there is any invincible brick
	for (int i = 0; i < da_count(bricks); i++)
	{
		BrickData* brickData = (BrickData*)pd->sprite->getUserdata(da_get(bricks, i));
		if (pd->sprite->isVisible(da_get(bricks, i)) && brickData->type != INVINCIBLE) return false;
	}

	return true;
}

void levelOver(void)
{
	//music();
	mode = LEVEL_OVER_WAIT;
	gameOverCountdown = 60;
	//blinkSpeed = 16;
}

/*********************
* Create Level Objects
**********************/
void setupWall(LCDSprite* wall, PDRect collisionRect, float pos_x, float pos_y)
{
	pd->sprite->setCollideRect(wall, collisionRect);
	pd->sprite->moveTo(wall, pos_x, pos_y);
	pd->sprite->setTag(wall, WALL);
	pd->sprite->setZIndex(wall, 1000);
	pd->sprite->addSprite(wall);
}

void createWalls(void)
{
	// I will use invisible sprites with collisions to check the collision with the border of the screens, 
	// this way I don't have to deal with screen collision and take advantage of Playdate collision system

	// Create Walls 
	walls.Top = pd->sprite->newSprite();
	walls.Bottom = pd->sprite->newSprite();
	walls.Right = pd->sprite->newSprite();
	walls.Left = pd->sprite->newSprite();

	int screen_width = pd->display->getWidth();
	int screen_height = pd->display->getHeight();

	// Top Wall
	PDRect top_collision_rect = PDRectMake(0, 0, (float)screen_width, 1.f);
	setupWall(walls.Top, top_collision_rect, 0.f, -1.f);

	// `Bottom Wall
	PDRect bottom_collision_rect = PDRectMake(0.f, 0.f, (float)screen_width, 1.f);
	setupWall(walls.Bottom, bottom_collision_rect, 0.f, (float)screen_height);

	// Right Wall
	PDRect right_collision_rect = PDRectMake(0.f, 0.f, 1.f, (float)screen_height);
	setupWall(walls.Right, right_collision_rect, (float)screen_width, 0.f);

	//Left Wall
	PDRect left_collision_rect = PDRectMake(0.f, 0.f, 1.f, (float)screen_height);
	setupWall(walls.Left, left_collision_rect, -1.f, 0.f);
}

void loadLevelBricks(void)
{
	da_free(bricks);
	da_init(bricks);
	char* currentLevelChr = LEVEL_get(currentLevel);

	// b = normal brick
	// x = empty space
	// i = indestructable brick
	// h = hardened brick
	// s = sploding brick
	// p = powerup brickf

	char currentBrick = '\0';
	char lastBrick = '\0';
	int gridPos = -1;

	// Iterate through all the chars in the current level char*
	for (int index = 0; index < strlen(currentLevelChr); ++index)
	{
		gridPos += 1;
		currentBrick = currentLevelChr[index];

		// Check if currentBrick is of any of this types
		if (currentBrick == 'i' ||
			currentBrick == 'b' ||
			currentBrick == 'h' ||
			currentBrick == 's' ||
			currentBrick == 'p')
		{
			da_push(bricks, BRICK_create(gridPos, currentBrick));
			lastBrick = currentBrick;
		}
		else if (currentBrick == 'x')
		{
			lastBrick = currentBrick;
		}
		else if (currentBrick == '/')
		{
			gridPos = (int)floor(((gridPos - 1) / 11) + 1) * 11;
		}
		else if (currentBrick >= '0' && currentBrick <= '9')
		{
			int tmp = atoi(&currentBrick);
			for (int j = 0; j < tmp; ++j)
			{
				if (lastBrick == 'i' ||
					lastBrick == 'b' ||
					lastBrick == 'h' ||
					lastBrick == 's' ||
					lastBrick == 'p')
				{
					da_push(bricks, BRICK_create(gridPos, lastBrick));
				}
				gridPos += 1;
			}
			gridPos -= 1;
		}
	}
}

/*************
* Serving Ball
*************/

// Reset balls and powerup timers to start with a new ball
void serveBall(void)
{
	// Get paddle position 
	float paddleX = 0;
	float paddleY = 0;
	pd->sprite->getPosition(paddle, &paddleX, &paddleY);
	PDRect paddleRect = pd->sprite->getCollideRect(paddle);

	// Reset all balls
	for (size_t i = 0; i < MAX_BALLS; ++i)
	{
		BALL_resetBall(balls[i]);
	}

	BALL_deactivate(balls[1]);
	BALL_deactivate(balls[2]);

	// Reset ball at paddle top center position
	BALL_activate(balls[0]);
	pd->sprite->moveTo(balls[0], PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS - (paddleRect.width * 0.5f));
	BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[0]);
	ballData->dx = 1.0f;
	ballData->dy = -1.0f;
	ballData->angle = 1.0f;
	ballData->isStuck = true;
	ballData->timerMega = 0;
	ballData->timerMegaWait = 0;
	ballData->timerSlow = 0;

	// Reset paddle timers
	PaddleData* paddleData = (PaddleData*)pd->sprite->getUserdata(paddle);
	paddleData->timerExpand = 0;
	paddleData->timerReduce = 0;
	paddleData->sticky = false;

	// Reset game variables
	suddenDeathBrick = NULL;
	scoreMul = 1;
	chain = 1;

	// Reset Pills
	resetPills();

	//sticky_x = 0;
}

// Make the ball stuck in the paddle to move
void GAME_releaseStuck(void)
{
	for (size_t i = 0; i < MAX_BALLS; ++i)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[i]);
		
		if (ballData->isStuck)
		{
			float x = 0;
			float y = 0;
			pd->sprite->getPosition(balls[i], &x, &y);
			pd->sprite->moveTo(balls[i], x, y);
			ballData->isStuck = false;
		}
	}
}

// Change the direction of the ball while in the paddle stucked
void pointStuck(int sign)
{
	for (size_t i = 0; i < MAX_BALLS; ++i)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[i]);

		if (ballData->isStuck)
		{
			float x = 0;
			float y = 0;
			pd->sprite->getPosition(balls[i], &x, &y);
			pd->sprite->moveTo(balls[i], x, y);
			ballData->dx = fabs(ballData->dx) * sign;
		}
	}
}

/*****************
* Score and Chains
******************/

// Increase chain combo multiplier
void boostChain(void)
{
	if (chain == 6)
	{
		int si = randomInt(0, MAX_SICK - 1);
		//sfx();
		//showSash(sick[si], -1, 1);
	}

	chain += 1;
	chain = (int)mid(1.f, (float)chain, 7.f);
}

// Calculate Points based on chain, fastMode and score multiplier
void getPoints(int points)
{
	PaddleData* paddleData = (BallData*)pd->sprite->getUserdata(paddle);

	if (fastMode)
	{
		points = points * 2;
	}

	if (paddleData->timerReduce <= 0)
	{
		points += points * chain * scoreMul;
	}
	else
	{
		points += points * chain * scoreMul * 10;
	}

	if (points > 10000)
	{
		points2 += 1;
		points -= 10000;
	}
}

/*******************
* Pills and Powerups
********************/
void multiball(void) 
{
	BallData* ballsData[MAX_BALLS];
	for (size_t i = 0; i < MAX_BALLS; ++i)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[i]);
		ballsData[i] = ballData;
	}

	LCDSprite* newBall = NULL;
	if (ballsData[1]->isDead)
	{
		BALL_copyBall(balls[0], balls[1]);
		ballsData[1]->isStuck = false;
		newBall = balls[1];
	}
	else if (ballsData[2]->isDead)
	{
		BALL_copyBall(balls[0], balls[2]);
		ballsData[2]->isStuck = false;
		newBall = balls[2];
	}
	
	if (newBall != NULL) 
	{
		if (FloatEquals(ballsData[0]->angle, 0.f))
		{
			BALL_setAngle(newBall, 2.f);
		}
		else if (FloatEquals(ballsData[0]->angle, 1.f))
		{
			BALL_setAngle(balls[0], 0.f);
			BALL_setAngle(newBall, 2.f);
		}
		else
		{
			BALL_setAngle(newBall, 0.f);
		}
	}
}

void GAME_getPowerup(LCDSprite* pill)
{
	if (pill)
	{
		PillData* pillData = (PillData*)pd->sprite->getUserdata(pill);
		BallData* ballsData[MAX_BALLS];
		PaddleData* paddleData = (PaddleData*)pd->sprite->getUserdata(paddle);

		for (size_t i = 0; i < MAX_BALLS; ++i)
		{
			BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[i]);
			ballsData[i] = ballData;
		}

		if (pillData)
		{
			switch (PILL_getType(pill))
			{
			case SLOW_DOWN:
				for (size_t i = 0; i < MAX_BALLS; ++i)
				{
					ballsData[i]->timerSlow = 400;
				}

				//showSash("slowdown!", 9, 4);
				break;
			case EXTRA_LIFE:
				lives += 1;
				//showSash("Extra Life!", 7, 6);
				break;
			case STICKY:
			{
				bool hasStuck = false;
				for (size_t i = 0; i < MAX_BALLS; ++i)
				{
					if (ballsData[i]->isStuck) 
					{
						hasStuck = true;
					}
				}

				if (!hasStuck) 
				{
					paddleData->sticky = true;
				}
				//showSash("Sticky Paddle!", 7, 6);
				break;
			}
			case EXPAND:
				paddleData->timerExpand = 600;
				paddleData->timerReduce = 0;
				//showSash("Expand!", 12, 1);
				break;
			case REDUCE:
				paddleData->timerReduce = 600;
				paddleData->timerExpand = 0;
				//showSash("Reduce!", 0, 8);
				break;
			case MEGABALL:
				for (size_t i = 0; i < MAX_BALLS; ++i)
				{
					ballsData[i]->timerMegaWait = 600;
					ballsData[i]->timerMega = 0;
				}
				//showSash("Megaball!", 8, 2);
				break;
			case MULTIBALL:
				multiball();
				//showSash("Multiball!", 8, 2);
				break;
			default:
				break;
			}
		}
	}
}

void resetPills(void)
{
	da_free(pills);
}

/***************************
* Start Background Particles
****************************/
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

/***************
* Process Inputs
****************/
unsigned int STARTSTATE_processInput(void)
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
		fastMode = !fastMode;
		//sfx();
	}

	if (current & kButtonRight && !buttonPressed)
	{
		// Show HighScore
		if (FloatEquals(highScoreDX, 128.f)) 
		{
			highScoreDX = 0.f;
			//sfx();
		}
	}

	if (current & kButtonLeft && !buttonPressed)
	{
		// Hide HighScore
		if (FloatEquals(highScoreDX, 0.f))
		{
			highScoreDX = 128.f;
			//sfx();
		}
	}
}

unsigned int GAMESTATE_processInput(void)
{
	PDButtons current;
	PDButtons currentReleased;
	pd->system->getButtonState(&current, NULL, &currentReleased);
	bool buttonPressed = false;

	PaddleData* paddleData = (PaddleData*)pd->sprite->getUserdata(paddle);
	if (current & kButtonLeft)
	{
		paddleData->dx = -PADDLE_DX;
		buttonPressed = true;
		pointStuck(-1);
	}
	if (current & kButtonRight)
	{
		paddleData->dx = PADDLE_DX;
		buttonPressed = true;
		pointStuck(1);
	}
	if (current & kButtonA)
	{
		GAME_releaseStuck();
	}

	// Slowdown paddle speed after release the button press
	if (!buttonPressed)
	{
		paddleData->dx = paddleData->dx / 1.3f;
		paddleData->speedWind = 0;
	}
	else
	{
		paddleData->speedWind += 1;
	}
}

unsigned int GAMEOVERSTATE_processInput(void) 
{
	PDButtons current;
	pd->system->getButtonState(NULL, NULL, &current);
	bool buttonPressed = false;

	if (current & kButtonA && !buttonPressed)
	{
		//Restart level
		gameOverCountdown = 80;
		gameOverRestart = true;
		//blinkSpeed = 1;
		//sfx();
	}

	if (current & kButtonB && !buttonPressed)
	{
		//Go To start
		gameOverCountdown = 80;
		gameOverRestart = false;
		//blinkSpeed = 1;
	}
}

unsigned int LEVELOVERSTATE_processInput(void)
{
	PDButtons current;
	pd->system->getButtonState(NULL, NULL, &current);
	bool buttonPressed = false;

	if (((current & kButtonA) || (current & kButtonRight)) && !buttonPressed)
	{
		//Restart level
		gameOverCountdown = 80;
		//blinkSpeed = 1;
		//sfx();
	}
}

unsigned int WINNERSTATE_processInput(void)
{
	PDButtons current;
	pd->system->getButtonState(NULL, NULL, &current);
	bool buttonPressed = false;

	if (logHighScore)
	{
		if ((current & kButtonLeft) && !buttonPressed)
		{
			//sfx(17)
			selectedInitial -= 1;
			if (selectedInitial < 0) 
			{
				selectedInitial = 3;
			}
		}

		if ((current & kButtonRight) && !buttonPressed)
		{
			//sfx(17)
			selectedInitial += 1;
			if (selectedInitial > 3)
			{
				selectedInitial = 0;
			}
		}

		if ((current & kButtonUp) && !buttonPressed)
		{
			if(selectedInitial < 3)
			{
				//sfx(17)
				initials[selectedInitial] -= 1;
				if (initials[selectedInitial] < 0)
				{
					selectedInitial = strlen(highScoreChars) - 1;
				}
			}
		}

		if ((current & kButtonDown) && !buttonPressed)
		{
			if (selectedInitial < 3)
			{
				//sfx(17)
				initials[selectedInitial] += 1;
				if (initials[selectedInitial] > strlen(highScoreChars))
				{
					selectedInitial = 0;
				}
			}
		}

		if ((current & kButtonA) && !buttonPressed)
		{
			if (selectedInitial == 3) 
			{
				//addHighScore(points, points2, initials[0], initials[1], initials[2]);
				//saveHighScore();
				gameOverCountdown = 80;
				//blinkspeed = 1;
				//sfx(15);
			}
		}
	}
	else 
	{
		if ((current & kButtonB) && !buttonPressed)
		{
			gameOverCountdown = 80;
			//blinkspeed = 1;
			//sfx(15);
		}
	}
}

unsigned int processInput(void) 
{
	switch (mode)
	{
	case START:
		STARTSTATE_processInput();
		break;
	case GAME:
		GAMESTATE_processInput();
		break;
	case GAME_OVER:
		GAMEOVERSTATE_processInput();
		break;
	case LEVEL_OVER:
		LEVELOVERSTATE_update();
		break;
	case WINNER:
		WINNERSTATE_update();
		break;
	default:
		break;
	}
}

/*************
* Create Game
*************/
unsigned int GAME_create(PlaydateAPI* p)
{
	// Init global variables
	pd = p;					// Set Playdate API to access globally
	mode = LOGO;
	deltaTime = 0.0f;

	// Init subsystems and resources
	RESOURCEMANAGER_load();						// Load resources
	PARTICLES_init();
	UTILS_init();

	// Load Fonts
	const char* err;
	font = pd->graphics->loadFont(fontpath, &err);
	if (font == NULL)
		pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

	return 0;
}

void goToStart(void) 
{
	mode = START;
	// Start particles in the background
	startBackgroundParticles();
	introCountdown = 0;
}

/******************
* Update functions
*******************/
void updateArrow(void)
{
	arrowMultiplayerFrame += 1;
	if (arrowMultiplayerFrame > 30)
	{
		arrowMultiplayerFrame = 0;
	}
	arrowMultiplier1 = 1.0f + (4.0f * (arrowMultiplayerFrame / 30.0f));

	int arrowMultiplayerFrameTmp = arrowMultiplayerFrame + 15;
	if (arrowMultiplayerFrameTmp > 30)
	{
		arrowMultiplayerFrameTmp = arrowMultiplayerFrameTmp - 30;
	}
	arrowMultiplier2 = 1.0f + (4.0f * (arrowMultiplayerFrameTmp / 30.0f));
}

/******************
* Update Functions
*******************/

unsigned int LOGOSTATE_update(void) 
{
	introCountdown -= 1;
	if (introCountdown < 0)
	{
		goToStart();
	}
	return 0;
}

unsigned int STARTSTATE_update(void)
{
	// Raining particles
	particlesTimer += 1;
	spawnBackgroundParticles(true, particlesTimer);

	// HighScore
	// TODO: should be removed and replace it with easing function
	if (!FloatEquals(highScoreX, highScoreDX)) 
	{
		highScoreX += (highScoreDX - highScoreX) / 5;
		if (fabs(highScoreDX - highScoreX) < 0.3f) 
		{
			highScoreX = highScoreDX;
		}
	}

	if (startCountdown < 0)
	{
		// Inputs
		STARTSTATE_processInput();
	}
	else
	{
		startCountdown -= 1;

		//fade();
		if (startCountdown <= 0)
		{
			startCountdown = -1;
			//blinkspeed = 8;

			PARTICLES_removeAllParticles();
			startGame();
		}
	}

	return 0;
}

unsigned int GAMESTATE_update(void)
{
	PaddleData* paddleData = (PaddleData*)pd->sprite->getUserdata(paddle);
	scoreMul = (paddleData->timerReduce > 0) ? 2 : 1;

	//Move Pills
	//Update Sudden Death
	//Check Explosion

	if (levelFinished())
	{
		//GAMESTATE_draw();
		if (currentLevel >= LEVEL_getAmount())
		{
			winGame();
		}
		else
		{
			levelOver();
		}
	}

	// Powerup Timers

	bool allBallsDead = true;
	// Check if balls are still alive
	for (size_t i = 0; i < MAX_BALLS; ++i)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[i]);
		if (!ballData->isDead)
		{
			allBallsDead = false;
		}
	}

	if (allBallsDead)
	{
		serveBall();
	}

	float pad_x = 0.f;
	float pad_y = 0.f;
	pd->sprite->getPosition(paddle, &pad_x, &pad_y);
	PDRect pad_bounds = pd->sprite->getBounds(paddle);

	// Check if ball is stuck
	for (size_t i = 0; i < MAX_BALLS; ++i)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[i]);
		if (ballData->isStuck)
		{
			PDRect ballRect = pd->sprite->getCollideRect(balls[i]);
			pd->sprite->moveTo(balls[i], pad_x, pad_y - ballRect.height);
			updateArrow();
		}
	}

	return 0;
}

unsigned int GAMEOVERSTATE_update(void)
{
	//float ang = randomFloat(0.0f, PI * 2.f);
	//float dx = sinf(ang) * randomFloat(0.0f, 0.6f);
	//float dy = cosf(ang) * randomFloat(0.0f, 0.6f);
	//LCDColor colors[4] = { ditheringPatterns[2], ditheringPatterns[2], ditheringPatterns[4], ditheringPatterns[8] };
	//int topRow = 120;
	//int btnRow = 162;
	//PARTICLES_addParticle(randomInt(0, 240), topRow, dx, dy, GRAVITY_SMOKE, 70.0f + randomFloat(0.0f, 15.0f), colors, 4, 6.0f + randomFloat(0.0f, 12.0f));
	//PARTICLES_addParticle(randomInt(0, 240), btnRow, dx, dy, GRAVITY_SMOKE, 70.0f + randomFloat(0.0f, 15.0f), colors, 4, 6.0f + randomFloat(0.0f, 12.0f));

	if (gameOverCountdown < 0)
	{
		GAMEOVERSTATE_processInput();
	}
	else
	{
		gameOverCountdown--;
		//fade
		if (gameOverCountdown <= 0)
		{
			if (gameOverRestart)
			{
				gameOverCountdown = -1;
				//blinkSpeed = 8;
				PARTICLES_removeAllParticles();
				restartLevel();
			}
			else
			{
				gameOverCountdown = -1;
				//blinkSpeed = 8;
				goToStart();
				highScoreX = 128.f;
				highScoreDX = 128.f;
				//music()
			}
		}
	}
	return 0;
}

unsigned int GAMEOVERWAITSTATE_update(void)
{
	gameOverCountdown--;
	if (gameOverCountdown <= 0) 
	{
		gameOverCountdown = -1;
		mode = GAME_OVER;
	}
}

unsigned int LEVELOVERSTATE_update(void)
{
	//float ang = randomFloat(0.0f, PI * 2.f);
	//float dx = sinf(ang) * randomFloat(0.0f, 0.3f);
	//float dy = cosf(ang) * randomFloat(0.0f, 0.3f);
	//
	//int topRow = 90;
	//int bottomRow = 100;
	//LCDColor colors[5] = { ditheringPatterns[0], ditheringPatterns[0], ditheringPatterns[6], ditheringPatterns[6], ditheringPatterns[10] };
	//PARTICLES_addParticle(randomInt(0, 400), topRow, dx, dy, GRAVITY_SMOKE, 70.0f + randomFloat(0.0f, 15.0f), colors, 5, 12 + randomFloat(0.0f, 10.0f));
	//PARTICLES_addParticle(randomInt(0, 400), bottomRow, dx, dy, GRAVITY_SMOKE, 70.0f + randomFloat(0.0f, 15.0f), colors, 5, 12 + randomFloat(0.0f, 10.0f));
		
	if (gameOverCountdown < 0) 
	{
		LEVELOVERSTATE_processInput();
	}
	else 
	{
		gameOverCountdown -= 1;
		//fade();
		if (gameOverCountdown <= 0) 
		{
			gameOverCountdown = -1;
			//blinkspeed = 8;
			//PARTICLES_removeAllParticles();
			POPUP_destroy(LevelFinishedUI);
			nextLevel();
		}
	}
}

unsigned int LEVELOVERWAITSTATE_update(void)
{
	gameOverCountdown -= 1;
	if (gameOverCountdown <= 0) 
	{
		gameOverCountdown = -1;
		mode = LEVEL_OVER;

		char* message = "STAGE CLEAR!!!\n\n\nPRESS A or -> to CONTINUE";
		LevelFinishedUI = POPUP_create(message, strlen(message));
	}
}

unsigned int WINNERSTATE_update(void)
{
	if (gameOverCountdown < 0)
	{
		WINNERSTATE_processInput();
	}
	else
	{
		gameOverCountdown -= 1;
		//fade();
		if (gameOverCountdown <= 0)
		{
			gameOverCountdown = -1;
			//blinkspeed = 8;
			PARTICLES_removeAllParticles();
			POPUP_destroy(LevelFinishedUI);
			goToStart();
			highScoreX = 128.f;
			highScoreDX = 0.f;
		}
	}
}

unsigned int WINNERWAITSTATE_update(void)
{
	gameOverCountdown -= 1;
	if (gameOverCountdown <= 0) 
	{
		gameOverCountdown = -1;
		//BlinkSpeed=4;
		mode = WINNER;
	}
}

unsigned int update(void) 
{
	// Update effects
	BLINK_update();
	SHAKE_update();
	PARTICLES_update();
	SASH_update();

	switch (mode)
	{
	case LOGO:
		LOGOSTATE_update();
		break;
	case START:
		STARTSTATE_update();
		break;
	case GAME:
		GAMESTATE_update();
		break;
	case GAME_OVER:
		GAMEOVERSTATE_update();
		break;
	case GAME_OVER_WAIT:
		GAMEOVERWAITSTATE_update();
		break;
	case LEVEL_OVER:
		LEVELOVERSTATE_update();
		break;
	case LEVEL_OVER_WAIT:
		LEVELOVERWAITSTATE_update();
		break;
	case WINNER:
		WINNERSTATE_update();
		break;
	case WINNER_WAIT:
		WINNERWAITSTATE_update();
		break;
	default:
		break;
	}

	return 0;
}

/******************
* Drawing functions
*******************/
void drawArrow(void)
{
	LCDSprite* currentBall = NULL;
	float ballX = 0;
	float ballY = 0;
	for (size_t i = 0; i < MAX_BALLS; ++i)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[i]);
		if (ballData->isStuck)
		{
			currentBall = balls[i];
			pd->sprite->getPosition(currentBall, &ballX, &ballY);
		}
	}

	if (currentBall)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(currentBall);

		int dot1PosX = ballX + ballData->dx * 6 * arrowMultiplier1;
		int dot1PosY = ballY + ballData->dy * 6 * arrowMultiplier1;
		pd->graphics->fillEllipse(dot1PosX - 2, dot1PosY - 2, 4, 4, 0.0f, 0.0f, kColorBlack);

		int dot2PosX = ballX + ballData->dx * 6 * arrowMultiplier2;
		int dot2PosY = ballY + ballData->dy * 6 * arrowMultiplier2;
		pd->graphics->fillEllipse(dot2PosX - 2, dot2PosY - 2, 4, 4, 0.0f, 0.0f, kColorBlack);
	}
}

/***************
* Draw Functions
****************/
unsigned int LOGOSTATE_draw(void)
{
	int x = (400 - TEXT_WIDTH) / 2;
	int y = (240 - TEXT_HEIGHT) / 2;
	pd->graphics->setFont(font);
	pd->graphics->drawText("LOGO!", strlen("LOGO!"), kASCIIEncoding, x, y);

	return 0;
}

unsigned int STARTSTATE_draw(void)
{
	// Draw Logo
	int textWidth = pd->graphics->getTextWidth(font, "Press A to start", 17, kASCIIEncoding, pd->graphics->getTextTracking());
	pd->graphics->drawText("Press A to start", 17, kASCIIEncoding, 200 - (textWidth * 0.5f), 160);
	SASH_draw();
	return 0;
}

unsigned int GAMESTATE_draw(void)
{
	pd->sprite->updateAndDrawSprites();

	// Check if ball is stuck
	for (size_t i = 0; i < MAX_BALLS; ++i)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[i]);
		if (ballData->isStuck)
		{
			drawArrow();
		}
	}

	SASH_draw();
	return 0;
}

unsigned int GAMEOVERSTATE_draw(void)
{
	pd->graphics->setBackgroundColor(kColorClear);

	// Draw Logo
	int textWidth = pd->graphics->getTextWidth(font, "GAME OVER STATE", 17, kASCIIEncoding, pd->graphics->getTextTracking());
	pd->graphics->drawText("GAME OVER STATE", 17, kASCIIEncoding, 200 - (textWidth * 0.5f), 160);

	return 0;
}

unsigned int GAMEOVERWAITSTATE_draw(void)
{
}

unsigned int LEVELOVERSTATE_draw(void)
{
	GAMESTATE_draw();

	//int textWidth1 = pd->graphics->getTextWidth(font, "STAGE CLEAR!!!", 12, kASCIIEncoding, pd->graphics->getTextTracking());
	//pd->graphics->drawText("STAGE CLEAR!!!", 12, kASCIIEncoding, 200 - (textWidth1 * 0.5f), 90);
	//int textWidth2 = pd->graphics->getTextWidth(font, "PRESS A or -> TO CONTINUE", 12, kASCIIEncoding, pd->graphics->getTextTracking());
	//pd->graphics->drawText("PRESS A TO CONTINUE", 12, kASCIIEncoding, 200 - (textWidth2 * 0.5f), 120);
}

unsigned int LEVELOVERWAITSTATE_draw(void)
{
	GAMESTATE_draw();
}

unsigned int WINNERSTATE_draw(void)
{
}

unsigned int WINNERWAITSTATE_draw(void)
{
}

unsigned int draw(void)
{
	// Clear screen
	pd->graphics->clear(kColorWhite);
	
	// Draw screen depending on State
	switch (mode)
	{
	case LOGO:
		LOGOSTATE_draw();
		break;
	case START:
		STARTSTATE_draw();
		break;
	case GAME:
		GAMESTATE_draw();
		break;
	case GAME_OVER:
		GAMEOVERSTATE_draw();
		break;
	case GAME_OVER_WAIT:
		GAMEOVERWAITSTATE_draw();
		break;
	case LEVEL_OVER:
		LEVELOVERSTATE_draw();
		break;
	case LEVEL_OVER_WAIT:
		LEVELOVERWAITSTATE_draw();
		break;
	case WINNER:
		WINNERSTATE_draw();
		break;
	case WINNER_WAIT:
		WINNERWAITSTATE_draw();
		break;
	default:
		break;
	}

	PARTICLES_draw();
	
	return 0;
}

/************
* Main Loop
************/
unsigned int GAME_update(void)
{
	// Main Loop
	processInput();
	update();
	draw();

	// Calculate deltaTime
	deltaTime = pd->system->getElapsedTime();
	pd->system->resetElapsedTime();

	return 0;
}

/*********************
* Destroy Game Objects
**********************/
unsigned int GAME_destroy(void)
{
	PARTICLES_removeAllParticles();
	pd->sprite->freeSprite(walls.Top);
	pd->sprite->freeSprite(walls.Bottom);
	pd->sprite->freeSprite(walls.Right);
	pd->sprite->freeSprite(walls.Left);

	PADDLE_destroy(paddle);
	for (size_t i = 0; i < MAX_BALLS; ++i)
	{
		BALL_destroy(balls[i]);
	}

	da_free(bricks);
	da_free(pills);

	return 0;
}

float GAME_deltaTime(void)
{
	return deltaTime;
}

/**************
* State Getters
**************/
LCDSprite* GAME_getPaddle(void)
{
	return paddle;
}

LCDSprite* GAME_getBall(void)
{
	return balls[0];
}

void GAME_resetChain()
{
	chain = 1;
}

static void updateSuddenDeath(void)
{
	if (suddenDeathBrick != NULL) 
	{
		BrickData* brickData = pd->sprite->getUserdata(suddenDeathBrick);
		suddenDeathTimer -= 1;
		if (suddenDeathTimer < 1) 
		{
			brickData->type = ZZ;
			suddenDeathBrick = NULL;
			return;
		}
		suddenDeathBlinkTimer -= 1;
		if (suddenDeathBlinkTimer < 1) 
		{
			brickData->flash = 6;
			suddenDeathBlinkTimer = suddenDeathTimer * 0.1f;
			//sfx(29)
		}
	}
}

static void triggerSuddenDeath(void) 
{
	for (int i = 0; i < da_count(bricks); i++)
	{
		LCDSprite* currentBrick = da_get(bricks, i);
		BrickData* brickData = pd->sprite->getUserdata(currentBrick);
		if (pd->sprite->isVisible(currentBrick) && brickData->type != INVINCIBLE)
		{
			suddenDeathBrick = currentBrick;
			SASH_show("sudden death!!", kColorBlack, kColorWhite);
			suddenDeathTimer = 450;
			suddenDeathBlinkTimer = suddenDeathTimer * 0.1f;
			brickData->flash = 6;
			//sfx(29);
		}
	}
}

void GAME_checkSuddenDeath(void)
{
	int c = 0;
	if (suddenDeathBrick == NULL) 
	{
		for (int i = 0; i < da_count(bricks); i++)
		{
			LCDSprite* currentBrick = da_get(bricks, i);
			BrickData* brickData = pd->sprite->getUserdata(currentBrick);
			if (pd->sprite->isVisible(currentBrick) && brickData->type != INVINCIBLE)
			{
				c += 1;
			}
			if (c > suddenDeathThresh)
			{
				return;
			}
		}

		if (c <= suddenDeathThresh)
		{
			triggerSuddenDeath();
		}
	}
}

void GAME_hitBrick(SpriteCollisionInfo* collision, bool combo)
{
	if (collision)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(collision->sprite);
		BrickData* brickData = (BrickData*)pd->sprite->getUserdata(collision->other);
		int flashTime = 1;
		if (brickData->type == SPLODING || collision->other == suddenDeathBrick)
		{
			BALL_megaballSmash(collision->sprite);
			ballData->infiniteCounter = 0.f;

			// Splosion brick
			// TODO: sfx(2 + chain);
			BRICK_shatterBrick(collision->other, ballData->lastHitX, ballData->lastHitY);
			brickData->type = ZZ;

			if (collision->other == suddenDeathBrick)
			{
				getPoints(10);
				suddenDeathBrick = NULL;
			}
			else
			{
				getPoints(1);
			}

			if (combo)
			{
				boostChain();
			}
		}
		else if (brickData->type == REGULAR)
		{
			BALL_megaballSmash(collision->sprite);
			ballData->infiniteCounter = 0.f;

			// Regular brick
			// TODO: sfx(2 + chain);
			BRICK_shatterBrick(collision->other, ballData->lastHitX, ballData->lastHitY);
			brickData->flash = flashTime;

			if (combo)
			{
				getPoints(1);
				boostChain();
			}
		}
		else if (brickData->type == INVINCIBLE)
		{
			//sfx();
		}
		else if (brickData->type == HARDENED)
		{
			BALL_megaballSmash(collision->sprite);
			ballData->infiniteCounter = 0.f;

			if (ballData->timerMega > 0)
			{
				//sfx();
				BRICK_shatterBrick(collision->other, ballData->lastHitX, ballData->lastHitY);
				brickData->flash = flashTime;

				if (combo)
				{
					getPoints(1);
					boostChain();
				}
			}
			else
			{
				//sfx();
				brickData->flash = flashTime;
				// Bump the brick
				brickData->dx = ballData->lastHitX * 0.25f;
				brickData->dy = ballData->lastHitY * 0.25f;

				brickData->hp--;
				if (brickData->hp <= 0)
				{
					brickData->type = REGULAR;
				}
			}
		}
		else if (brickData->type == POWER)
		{
			BALL_megaballSmash(collision->sprite);
			ballData->infiniteCounter = 0;

			// Powerup Brick
			//sfx();
			BRICK_shatterBrick(collision->other, ballData->lastHitX, ballData->lastHitY);
			brickData->flash = flashTime;

			if (combo)
			{
				getPoints(1);
				boostChain();
			}

			float x = 0.0f;
			float y = 0.0f;
			pd->sprite->getPosition(collision->other, &x, &y);
			PILL_create(x, y);
		}
	}
}

void GAME_explodeBrick(LCDSprite* sprite)
{
	if (sprite)
	{
		BrickData* brickData = (BrickData*)pd->sprite->getUserdata(sprite);
		int exploding_x = 0;
		int exploding_y = 0;
		pd->sprite->getPosition(sprite, &exploding_x, &exploding_y);
		PDRect brickRect = pd->sprite->getBounds(sprite);
		//pd->sprite->setVisible(sprite, 0);
		//brickData->visible = false;

		for (int i = 0; i < da_count(bricks); i++)
		{
			LCDSprite* currentBrick = da_get(bricks, i);
			int current_x = 0;
			int current_y = 0;
			pd->sprite->getPosition(currentBrick, &current_x, &current_y);

			if (sprite != currentBrick &&
				pd->sprite->isVisible(currentBrick) &&
				abs(current_x - exploding_x) <= (brickRect.width + 4) &&
				abs(current_y - exploding_y) <= (brickRect.height + 4))
			{
				//SpriteCollisionInfo collision = { .sprite = balls[0], .other = sprite };
				//GAME_hitBrick(, false);
			}
		}
	}
}
