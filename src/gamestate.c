#include "gamestate.h"

#include "pd_api.h"

#include "memory.h"
#include "utils.h"
#include "raymath.h"

#include "particles.h"
#include "patterns.h"
#include "DG_dynarr.h"

#include "level.h"
#include "paddle.h"
#include "ball.h"
#include "brick.h"
#include "pill.h"

#include <stdbool.h>

#define MAX_BALLS 3
#define MAX_SICK 12

DA_TYPEDEF(LCDSprite*, BricksArrayType);

typedef struct 
{
	LCDSprite* Top;
	LCDSprite* Bottom;
	LCDSprite* Right;
	LCDSprite* Left;
} Walls;

typedef struct
{
    // Game objects
	LCDSprite* paddle;
	LCDSprite* ball;
	BricksArrayType bricks;
	LCDSprite* suddenDeathBrick;

	Walls walls;

	//Levels
    int currentLevel;

    EMode nextState;

	bool sticky;

	int lives;
	int chain;
	int scoreMul;
	int points;
	int points2;
	int fastMode;

	int gameOverCountdown;
	int blinkSpeed;
} GameState;

static GameState* state = NULL;
static char* levels[MAX_SICK] = {
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

void boostChain()
{
	if (state->chain == 6)
	{
		int si = randomInt(0, MAX_SICK - 1);
		//sfx();
		//showSash();
	}

	state->chain += 1;
	state->chain = (int)mid(1.f, (float)state->chain, 7.f);
}

void getPoints(int points)
{
	PlaydateAPI* pd = getPlaydateAPI();
	PaddleData* paddleData = (BallData*)pd->sprite->getUserdata(state->paddle);

	if (state->fastMode)
	{
		points = points * 2;
	}

	if (paddleData->timerReduce <= 0)
	{
		state->points += points * state->chain * state->scoreMul;
	}
	else
	{
		state->points += points * state->chain * state->scoreMul * 10;
	}

	if (state->points > 10000)
	{
		state->points2 += 1;
		state->points -= 10000;
	}
}

void GAMESTATE_getPowerup(LCDSprite* pill)
{
	if (pill) 
	{
		PlaydateAPI* pd = getPlaydateAPI();
		PillData* pillData = (PillData*)pd->sprite->getUserdata(pill);
		BallData* ballData = (BallData*)pd->sprite->getUserdata(state->ball);
		PaddleData* paddleData = (PaddleData*)pd->sprite->getUserdata(state->paddle);

		if (pillData) 
		{
			switch (PILL_getType(pill))
			{
			case SLOW_DOWN:
				ballData->timerSlow = 400;
				//showSash("slowdown!", 9, 4);
				break;
			case EXTRA_LIFE:
				state->lives += 1;
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
				ballData->timerMegaWait = 600;
				ballData->timerMega = 0;
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

void setupWall(LCDSprite* wall, PDRect collisionRect, float pos_x, float pos_y)
{
    PlaydateAPI* pd = getPlaydateAPI();

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
	PlaydateAPI* pd = getPlaydateAPI();
	state->walls.Top = pd->sprite->newSprite();
	state->walls.Bottom = pd->sprite->newSprite();
	state->walls.Right = pd->sprite->newSprite();
	state->walls.Left = pd->sprite->newSprite();

    int screen_width = pd->display->getWidth();
	int screen_height = pd->display->getHeight();

	// Top Wall
	PDRect top_collision_rect = PDRectMake(0, 0, (float)screen_width, 1.f);
    setupWall(state->walls.Top, top_collision_rect, 0.f, -1.f);

	// `Bottom Wall
    PDRect bottom_collision_rect = PDRectMake(0.f, 0.f, (float)screen_width, 1.f);
    setupWall(state->walls.Bottom, bottom_collision_rect, 0.f, (float)screen_height);

	// Right Wall
    PDRect right_collision_rect = PDRectMake(0.f, 0.f, 1.f, (float)screen_height);
    setupWall(state->walls.Right, right_collision_rect, (float)screen_width, 0.f);

	//Left Wall
    PDRect left_collision_rect = PDRectMake(0.f, 0.f, 1.f, (float)screen_height);
    setupWall(state->walls.Left, left_collision_rect, -1.f, 0.f);
}

void serveBall(void)
{
	PlaydateAPI* pd = getPlaydateAPI();

	// Get paddle position 
	float paddleX = 0;
	float paddleY = 0;
	pd->sprite->getPosition(state->paddle, &paddleX, &paddleY);
	PDRect paddleRect = pd->sprite->getCollideRect(state->paddle);
    
    // Create new ball at paddle top center position
    state->ball = BALL_create(paddleX, paddleY - (paddleRect.width * 0.5));
	BallData* ballData = (BallData*)pd->sprite->getUserdata(state->ball);
	ballData->dx = 1.0f;
	ballData->dy = -1.0f;
	ballData->angle = 1.0f;
	ballData->isStuck = true;

	state->scoreMul = 1;
}

void releaseStuck(void) 
{
	PlaydateAPI* pd = getPlaydateAPI();
	BallData* ballData = (BallData*)pd->sprite->getUserdata(state->ball);

	if (ballData->isStuck)
	{
		float x = 0;
		float y = 0;
		pd->sprite->getPosition(state->ball, &x, &y);
		pd->sprite->moveTo(state->ball, x, y);
		ballData->isStuck = false;
	}
}

void loadLevelBricks(void)
{
	char* currentLevel = LEVEL_get(state->currentLevel);

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
	for (int index = 0; index < strlen(currentLevel); ++index)
	{
		gridPos += 1;
		currentBrick = currentLevel[index];

        // Check if currentBrick is of any of this types
		if (currentBrick == 'i' ||
			currentBrick == 'b' ||
			currentBrick == 'h' ||
			currentBrick == 's' ||
			currentBrick == 'p') 
		{
			da_push(state->bricks, BRICK_create(gridPos, currentBrick));
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
					da_push(state->bricks, BRICK_create(gridPos, lastBrick));
				}
				gridPos += 1;
			}
			gridPos -= 1;
		}
	}
}

bool levelFinished()
{
	PlaydateAPI* pd = getPlaydateAPI();

	bool returnValue = false;

	if (da_count(state->bricks) == 0)
	{
		return true;
	}

	// checks if there is any invincible brick
	for (int i = 0; i < da_count(state->bricks); i++)
	{
		BrickData* brickData = (BrickData*)pd->sprite->getUserdata(da_get(state->bricks, i));
		if(pd->sprite->isVisible(da_get(state->bricks, i)) && brickData->type != INVINCIBLE) return false;
	}

	return true;
}

void nextLevel()
{
	getPlaydateAPI()->sprite->moveTo(state->paddle, PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS);
	state->currentLevel += 1;
	
	if (state->currentLevel > LEVEL_getAmount())
	{

	}
	else
	{
		loadLevelBricks();
		serveBall();
	}
}

void restartLevel()
{
	// Create walls, ball and bricks
	getPlaydateAPI()->sprite->moveTo(state->paddle, PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS);
    createWalls();
	da_init(state->bricks);
	loadLevelBricks();
	serveBall();
}

void startGame()
{
	// Init game data
    state->currentLevel = 0;
    state->nextState = GAME;

	state->paddle = PADDLE_create(PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS);

	restartLevel();
}

void winGame() 
{
	state->nextState = WINNER_WAIT;
	state->gameOverCountdown = 60;
	state->blinkSpeed = 16;

	//Manage HighScore
}

void gameOver()
{
	state->nextState = GAME_OVER_WAIT;
	state->gameOverCountdown = 60;
	state->blinkSpeed = 16;
}

void levelOver() 
{
	state->nextState = LEVEL_OVER_WAIT;
	state->gameOverCountdown = 60;
	state->blinkSpeed = 16;
}

void GAMESTATE_processInput(void)
{
    PlaydateAPI* pd = getPlaydateAPI();

	PDButtons current;
	PDButtons currentReleased;
	pd->system->getButtonState(&current, NULL, &currentReleased);
	bool buttonPressed = false;

	PaddleData* paddleData = (PaddleData*)pd->sprite->getUserdata(state->paddle);
	if (current & kButtonLeft)
	{
		paddleData->dx = -PADDLE_DX;
		buttonPressed = true;
	}
	if (current & kButtonRight)
	{
		paddleData->dx = PADDLE_DX;
		buttonPressed = true;
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

void GAMESTATE_setNextState(EMode mode)
{
	state->nextState = mode;
}

EMode GAMESTATE_getNextState(void)
{
    return state->nextState;
}

unsigned int GAMESTATE_init(void)
{
	PlaydateAPI* pd = getPlaydateAPI();
    
	// Create GameState
    state = pd_malloc(sizeof(GameState));

	PARTICLES_init();
	
	startGame();

    return 0;
}

unsigned int GAMESTATE_update(float deltaTime)
{
	PlaydateAPI* pd = getPlaydateAPI();
	PaddleData* paddleData = (PaddleData*)pd->sprite->getUserdata(state->paddle);
	if (paddleData->timerReduce > 0)
	{
		state->scoreMul = 2;
	}
	else 
	{
		state->scoreMul = 1;
	}

    GAMESTATE_processInput();

	//Move Pills
	//Update Sudden Death
	//Check Explosion

	if (levelFinished()) 
	{
		//GAMESTATE_draw();
		if (state->currentLevel >= LEVEL_getAmount()) 
		{
			winGame();
		}
		else 
		{
			levelOver();
		}
	}

	// Powerup Timers
	

    // Check if ball still alive
	BallData* ballData = (BallData*)pd->sprite->getUserdata(state->ball);
	if (ballData->isDead)
	{
		BALL_destroy(state->ball);
		serveBall();
	}

	float pad_x = 0.f;
	float pad_y = 0.f;
	pd->sprite->getPosition(state->paddle, &pad_x, &pad_y);
	PDRect pad_bounds = pd->sprite->getBounds(state->paddle);

    // Check if ball is stuck
    if (ballData->isStuck)
	{
		PDRect ballRect = pd->sprite->getCollideRect(state->ball);
		pd->sprite->moveTo(state->ball, pad_x, pad_y - ballRect.height);
	}

    return 0;
}

unsigned int GAMESTATE_draw(float deltaTime)
{
	PlaydateAPI* pd = getPlaydateAPI();
	pd->graphics->setBackgroundColor(kColorClear);

	pd->sprite->updateAndDrawSprites();

    return 0;
}

unsigned int GAMESTATE_destroy(void)
{
    PlaydateAPI* pd = getPlaydateAPI();
	pd->sprite->freeSprite(state->walls.Top);
	pd->sprite->freeSprite(state->walls.Bottom);
	pd->sprite->freeSprite(state->walls.Right);
	pd->sprite->freeSprite(state->walls.Left);

	PADDLE_destroy(state->paddle);
	BALL_destroy(state->ball);

	da_free(state->bricks);

    pd_free(state);
    state = NULL;

    return 0;
}


LCDSprite* GAMESTATE_getPaddle(void)
{
	return state->paddle;
}

LCDSprite* GAMESTATE_getBall(void)
{
	return state->ball;
}

void GAMESTATE_resetChain()
{
	state->chain = 1;
}

void GAMESTATE_checkSD(void)
{
	
}

void GAMESTATE_hitBrick(SpriteCollisionInfo* collision, bool combo)
{
	if (collision)
	{
		PlaydateAPI* pd = getPlaydateAPI();
		BallData* ballData = (BallData*)pd->sprite->getUserdata(collision->sprite);
		BrickData* brickData = (BrickData*)pd->sprite->getUserdata(collision->other);
		int flashTime = 10;
		if (brickData->type == SPLODING || collision->other == state->suddenDeathBrick)
		{
			BALL_megaballSmash(collision->sprite);
			ballData->infiniteCounter = 0;

			// Splosion brick
			// TODO: sfx(2 + chain);
			BRICK_shatterBrick(collision->other, ballData->lastHitDx, ballData->lastHitDy);
			brickData->type = ZZ;

			if (collision->other == state->suddenDeathBrick) 
			{
				//TODO: getPoints(10);
				state->suddenDeathBrick = NULL;
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