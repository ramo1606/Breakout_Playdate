#include "gamestate.h"
#include "common.h"

#include "memory.h"
#include "utils.h"
#include "raymath.h"
#include "particles.h"
#include "DG_dynarr.h"
#include "level.h"
#include "paddle.h"
#include "ball.h"
#include "brick.h"
#include "pill.h"

#define MAX_BALLS 3
#define MAX_SICK 12
#define START_LIVES 4

DA_TYPEDEF(LCDSprite*, BricksArrayType);
DA_TYPEDEF(LCDSprite*, PillsArrayType);

typedef struct 
{
	LCDSprite* Top;
	LCDSprite* Bottom;
	LCDSprite* Right;
	LCDSprite* Left;
} Walls;

// Game Entities
LCDSprite* paddle = NULL;
LCDSprite* balls[MAX_BALLS];
BricksArrayType bricks;
PillsArrayType pills;

Walls walls;

// Suden Death
LCDSprite* suddenDeathBrick = NULL;
int suddenDeathTimer = 0;
int suddenDeathThresh = 3;

// Arrow
float arrowMultiplier1 = 1;
float arrowMultiplier2 = 1;
int arrowMultiplayerFrame = 0;

// Levels
int currentLevel = 0;

// Player Data
int lives = START_LIVES;
int chain = 1;
int scoreMul = 1;
int points = 0;
int points2 = 0;

// Other
int gameOverCountdown;

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
void processInput(void);
void createWalls(void);
void updateArrow(void);
void drawArrow(void);

/*************
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
	gameOverCountdown = 60;
	//blinkSpeed = 16;

	//Manage HighScore
}

void gameOver(void)
{
	gameOverCountdown = 60;
	//blinkSpeed = 16;
}

/*************
* Levels Flow
**************/
void restartLevel(void)
{
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

	//ShowSash();

	serveBall();
}

void nextLevel(void)
{
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
	//resetPills();
}

// Make the ball stuck in the paddle to move
void releaseStuck(void)
{
	BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[0]);

	if (ballData->isStuck)
	{
		float x = 0;
		float y = 0;
		pd->sprite->getPosition(balls[0], &x, &y);
		pd->sprite->moveTo(balls[0], x, y);
		ballData->isStuck = false;
	}
}

// Change the direction of the ball while in the paddle stucked
void pointStuck(int sign)
{
	BallData* ballData = (BallData*)pd->sprite->getUserdata(balls[0]);

	if (ballData->isStuck)
	{
		float x = 0;
		float y = 0;
		pd->sprite->getPosition(balls[0], &x, &y);
		pd->sprite->moveTo(balls[0], x, y);
		ballData->dx = fabs(ballData->dx) * sign;
	}
}

/*****************
* Score and Chains
******************/

// Increase chain combo multiplier
void boostChain()
{
	if (chain == 6)
	{
		int si = randomInt(0, MAX_SICK - 1);
		//sfx();
		//showSash();
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
void GAMESTATE_getPowerup(LCDSprite* pill)
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
				//TODO: logick for sticky paddle
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
				//multiball();
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

/*****************
* State functions
*****************/
unsigned int GAMESTATE_init(void)
{	
	startGame();

    return 0;
}

unsigned int GAMESTATE_update(float deltaTime)
{
	PaddleData* paddleData = (PaddleData*)pd->sprite->getUserdata(paddle);
	scoreMul = (paddleData->timerReduce > 0) ? 2 : 1;

    processInput();

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

unsigned int GAMESTATE_draw(float deltaTime)
{
	pd->graphics->setBackgroundColor(kColorClear);

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

    return 0;
}

unsigned int GAMESTATE_destroy(void)
{
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

/**************
* State Getters
**************/

LCDSprite* GAMESTATE_getPaddle(void)
{
	return paddle;
}

LCDSprite* GAMESTATE_getBall(void)
{
	return balls[0];
}

void GAMESTATE_resetChain(void)
{
	chain = 1;
}

void GAMESTATE_checkSD(void)
{
	
}

void GAMESTATE_hitBrick(SpriteCollisionInfo* collision, bool combo)
{
	if (collision)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(collision->sprite);
		BrickData* brickData = (BrickData*)pd->sprite->getUserdata(collision->other);
		int flashTime = 10;
		if (brickData->type == SPLODING || collision->other == suddenDeathBrick)
		{
			BALL_megaballSmash(collision->sprite);
			ballData->infiniteCounter = 0;

			// Splosion brick
			// TODO: sfx(2 + chain);
			BRICK_shatterBrick(collision->other, ballData->lastHitDx, ballData->lastHitDy);
			brickData->type = ZZ;

			if (collision->other == suddenDeathBrick) 
			{
				//TODO: getPoints(10);
				suddenDeathBrick = NULL;
			}
			else 
			{
				//TODO: getPoints(1);
			}

			if (combo) 
			{
				//TODO: boostChain();
			}
		}
		else if (brickData->type == REGULAR)
		{
			BALL_megaballSmash(collision->sprite);
			ballData->infiniteCounter = 0;

			// Regular brick
			// TODO: sfx(2 + chain);
			BRICK_shatterBrick(collision->other, ballData->lastHitDx, ballData->lastHitDy);
			brickData->flash = flashTime;
			brickData->visible = false;

			if (combo) 
			{
				//getPoints(1);
				//boostChain();
			}
		}
		else if (brickData->type == INVINCIBLE)
		{
			//sfx();
		}
		else if (brickData->type == HARDENED)
		{
			BALL_megaballSmash(collision->sprite);
			ballData->infiniteCounter = 0;

			if (ballData->timerMega > 0) 
			{
				//sfx();
				BRICK_shatterBrick(collision->other, ballData->lastHitDx, ballData->lastHitDy);
				brickData->flash = flashTime;
				brickData->visible = false;

				if (combo)
				{
					//getPoints(1);
					//boostChain();
				}
			}
			else 
			{
				//sfx();
				brickData->flash = flashTime;
				// Bump the brick
				brickData->dx = ballData->lastHitDx * 0.25f;
				brickData->dy = ballData->lastHitDy * 0.25f;

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

			//sfx();
			BRICK_shatterBrick(collision->other, ballData->lastHitDx, ballData->lastHitDy);
			brickData->flash = flashTime;
			brickData->visible = false;

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

void processInput(void)
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
		releaseStuck();
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