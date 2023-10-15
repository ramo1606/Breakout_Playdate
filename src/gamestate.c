#include "gamestate.h"

#include "pd_api.h"

#include "memory.h"
#include "utils.h"

#include "particles.h"

#include "level.h"
#include "paddle.h"
#include "ball.h"
#include "brick.h"

#include <stdbool.h>

#define MAX_BALLS 3

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
	LCDSprite** bricks;

	Walls walls;

	//Levels
    int currentLevel;

    EMode nextState;

	bool sticky;

	//Particles: move to particle files
	float lastHitDX;
	float lastHitDY;
} GameState;

static GameState* state = NULL;

//void spawnSpeedLines(float x, float y)
//{
//	PlaydateAPI* pd = getPlaydateAPI();
//
//	if ((float)rand() / (float)RAND_MAX < 0.2f)
//	{
//		PDRect pad_bounds = pd->sprite->getBounds(state->paddle);
//		float ox = ((float)rand() / (float)RAND_MAX) * 2.5f;
//		float oy = ((float)rand() / (float)RAND_MAX) * pad_bounds.height;
//
//		PARTICLES_addParticle(x + ox, (y - pad_bounds.height * 0.5f) + oy, PADDLE_getDx(state->paddle), 0.f, SPEED_LINE, 10.f + (float)(rand() % 15), kColorBlack, 2.f + (float)(rand() % 4));
//	}
//}

//void spawnPuft(float x, float y)
//{
//	PlaydateAPI* pd = getPlaydateAPI();
//
//	for (int i = 0; i < 5; i++) 
//	{
//		float ang = (float)rand() / (float)RAND_MAX;
//		float dx = sin(ang) * 1;
//		float dy = cos(ang) * 1;
//		PARTICLES_addParticle(x, y, dx, dy, SMOKE_BALL, (float)(rand() % 15), kColorBlack, 1 + (float)(rand() % 2));
//	}
//}

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
	float paddle_x = 0;
	float paddle_y = 0;
	pd->sprite->getPosition(state->paddle, &paddle_x, &paddle_y);
	PDRect paddle_rect = pd->sprite->getCollideRect(state->paddle);
    
    // Create new ball at paddle top center position
    state->ball = BALL_create(paddle_x, paddle_y - (paddle_rect.width * 0.5));
	BALL_setDx(state->ball, 1.f);
	BALL_setDy(state->ball, -1.f);
	BALL_setAngle(state->ball, 1.f);
	BALL_setStuck(state->ball, true);
}

void releaseStuck(void) 
{
	PlaydateAPI* pd = getPlaydateAPI();

	if (BALL_isStuck(state->ball))
	{
		float x = 0;
		float y = 0;
		pd->sprite->getPosition(state->ball, &x, &y);
		pd->sprite->moveTo(state->ball, x, y);
		BALL_setStuck(state->ball, false);
	}
}

void loadLevelBricks(void)
{
	char* currentLevel = LEVEL_get(state->currentLevel);
	state->bricks = pd_malloc(sizeof(LCDSprite*) * strlen(currentLevel));

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
			state->bricks[index] = BRICK_create(gridPos, currentBrick);
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
					state->bricks[index] = BRICK_create(gridPos, lastBrick);
				}
				gridPos += 1;
			}
			gridPos -= 1;
		}
	}
}

bool levelFinished()
{
	bool returnValue = false;

	int bricksSize = sizeof(state->bricks)/sizeof(state->bricks[0]);
	if (bricksSize == 0)
	{
		return true;
	}

	// checks if there is any invincible brick
	for (int i = 0; i < bricksSize; i++)
	{
		if(/*state->bricks[i].v == true*/BRICK_getType(state->bricks[i]) != INVINCIBLE) return false;
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

void GAMESTATE_processInput(void)
{
    PlaydateAPI* pd = getPlaydateAPI();

	PDButtons current;
	pd->system->getButtonState(&current, NULL, NULL);
	bool buttonPressed = false;

	if (current & kButtonLeft)
	{
		PADDLE_setDx(state->paddle, -PADDLE_DX);
		buttonPressed = true;
	}
	if (current & kButtonRight)
	{
		PADDLE_setDx(state->paddle, PADDLE_DX);
		buttonPressed = true;
	}
	if (current & kButtonA) 
	{
		releaseStuck();
	}

	//if (current & kButtonB)
	//{
	//	spawnPuft(40.f, 40.f);
	//}
	
	if (!buttonPressed)
	{
		PADDLE_setDx(state->paddle, PADDLE_getDx(state->paddle) / 1.3f);
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

	/* Intializes random number generator */
	srand(pd->system->getSecondsSinceEpoch(NULL));
	
	startGame();

    return 0;
}

unsigned int GAMESTATE_update(float deltaTime)
{
	PlaydateAPI* pd = getPlaydateAPI();
    
    GAMESTATE_processInput();

    // Check if ball still alive
	if (BALL_isDead(state->ball))
	{
		BALL_destroy(state->ball);
		serveBall();
	}

	float pad_x = 0.f;
	float pad_y = 0.f;
	pd->sprite->getPosition(state->paddle, &pad_x, &pad_y);
	PDRect pad_bounds = pd->sprite->getBounds(state->paddle);

    // Check if ball is stuck
    if (BALL_isStuck(state->ball))
	{
		PDRect ball_rect = pd->sprite->getCollideRect(state->ball);
		pd->sprite->moveTo(state->ball, pad_x, pad_y - ball_rect.height);
	}

	//if (!areEqual(PADDLE_getDx(state->paddle), 0.f)) 
	//{
	//	if (PADDLE_getDx(state->paddle) > 0.f)
	//	{
	//		spawnSpeedLines(pad_x - ((pad_bounds.width * 0.5f) - 2.5f), pad_y);
	//	}
	//	else
	//	{
	//		spawnSpeedLines(pad_x + (pad_bounds.width * 0.5f) + 2.5f, pad_y);
	//	}
	//}

	//PARTICLES_update();

    return 0;
}

unsigned int GAMESTATE_draw(float deltaTime)
{
	PlaydateAPI* pd = getPlaydateAPI();
	pd->graphics->clear(kColorWhite);

	pd->sprite->updateAndDrawSprites();
	//PARTICLES_draw();

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