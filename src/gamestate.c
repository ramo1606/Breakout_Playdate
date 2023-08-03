#include "gamestate.h"

#include "pd_api.h"

#include "memory.h"

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
} GameState;

static GameState* game = NULL;

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
	game->walls.Top = pd->sprite->newSprite();
	game->walls.Bottom = pd->sprite->newSprite();
	game->walls.Right = pd->sprite->newSprite();
	game->walls.Left = pd->sprite->newSprite();

    int screen_width = pd->display->getWidth();
	int screen_height = pd->display->getHeight();

	// Top Wall
	PDRect top_collision_rect = PDRectMake(0, 0, (float)screen_width, 1.f);
    setupWall(game->walls.Top, top_collision_rect, 0.f, -1.f);

	// `Bottom Wall
    PDRect bottom_collision_rect = PDRectMake(0.f, 0.f, (float)screen_width, 1.f);
    setupWall(game->walls.Bottom, bottom_collision_rect, 0.f, (float)screen_height);

	// Right Wall
    PDRect right_collision_rect = PDRectMake(0.f, 0.f, 1.f, (float)screen_height);
    setupWall(game->walls.Right, right_collision_rect, (float)screen_width, 0.f);

	//Left Wall
    PDRect left_collision_rect = PDRectMake(0.f, 0.f, 1.f, (float)screen_height);
    setupWall(game->walls.Left, left_collision_rect, -1.f, 0.f);
}

void serveBall(void)
{
	PlaydateAPI* pd = getPlaydateAPI();

	// Get paddle position 
	float paddle_x = 0;
	float paddle_y = 0;
	pd->sprite->getPosition(game->paddle, &paddle_x, &paddle_y);
	PDRect paddle_rect = pd->sprite->getCollideRect(game->paddle);
    
    // Create new ball at paddle top center position
    game->ball = BALL_create(paddle_x, paddle_y - (paddle_rect.width / 2));
	BALL_setDx(game->ball, 1.f);
	BALL_setDy(game->ball, -1.f);
	BALL_setAngle(game->ball, 1.f);
	BALL_setStuck(game->ball, true);
}

void releaseStuck(void) 
{
	PlaydateAPI* pd = getPlaydateAPI();

	if (BALL_isStuck(game->ball))
	{
		float x = 0;
		float y = 0;
		pd->sprite->getPosition(game->ball, &x, &y);
		pd->sprite->moveTo(game->ball, x, y);
		BALL_setStuck(game->ball, false);
	}
}

void loadLevelBricks(void)
{
	char* currentLevel = LEVEL_get(game->currentLevel);
	game->bricks = pd_malloc(sizeof(LCDSprite*) * strlen(currentLevel));

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
			game->bricks[index] = BRICK_create(gridPos, currentBrick);
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
					game->bricks[index] = BRICK_create(gridPos, lastBrick);
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

	int bricksSize = sizeof(game->bricks)/sizeof(game->bricks[0]);
	getPlaydateAPI()->system->logToConsole("%i", bricksSize);
	if (bricksSize == 0)
	{
		return true;
	}

	// checks if there is any invincible brick
	for (int i = 0; i < bricksSize; i++)
	{
		if(/*game->bricks[i].v == true*/BRICK_getType(game->bricks[i]) != INVINCIBLE) return false;
	}

	return true;
}

void nextLevel()
{
	getPlaydateAPI()->sprite->moveTo(game->paddle, PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS);
	game->currentLevel += 1;
	
	if (game->currentLevel > LEVEL_getAmount())
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
	getPlaydateAPI()->sprite->moveTo(game->paddle, PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS);
    createWalls();
	loadLevelBricks();
	serveBall();
}

void startGame()
{
	// Init game data
    game->currentLevel = 0;
    game->nextState = GAME;

	game->paddle = PADDLE_create(PADDLE_INITIAL_X_POS, PADDLE_INITIAL_Y_POS);

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
		PADDLE_setDx(game->paddle, -PADDLE_DX);
		buttonPressed = true;
	}
	if (current & kButtonRight)
	{
		PADDLE_setDx(game->paddle, PADDLE_DX);
		buttonPressed = true;
	}
	if (current & kButtonA) 
	{
		releaseStuck();
	}
	
	if (!buttonPressed)
	{
		PADDLE_setDx(game->paddle, PADDLE_getDx(game->paddle) / 1.3f);
	}
}

void GAMESTATE_setNextState(EMode mode)
{
	game->nextState = mode;
}

EMode GAMESTATE_getNextState(void)
{
    return game->nextState;
}

unsigned int GAMESTATE_init(void)
{
    // Create GameState
    game = pd_malloc(sizeof(GameState));
	startGame();

    return 0;
}

unsigned int GAMESTATE_update(float deltaTime)
{
	PlaydateAPI* pd = getPlaydateAPI();
    
    GAMESTATE_processInput();

    // Check if ball still alive
	if (BALL_isDead(game->ball))
	{
		BALL_destroy(game->ball);
		serveBall();
	}

    // Check if ball is stuck
    if (BALL_isStuck(game->ball))
	{
		float pad_x = 0.f;
		float pad_y = 0.f;
		pd->sprite->getPosition(game->paddle, &pad_x, &pad_y);

		PDRect ball_rect = pd->sprite->getCollideRect(game->ball);
		pd->sprite->moveTo(game->ball, pad_x, pad_y - ball_rect.height);
		//pd->sprite->markDirty(game->ball);
	}

	pd->sprite->updateAndDrawSprites();

    return 0;
}

unsigned int GAMESTATE_draw(float deltaTime)
{
    return 0;
}

unsigned int GAMESTATE_destroy(void)
{
    PlaydateAPI* pd = getPlaydateAPI();
	pd->sprite->freeSprite(game->walls.Top);
	pd->sprite->freeSprite(game->walls.Bottom);
	pd->sprite->freeSprite(game->walls.Right);
	pd->sprite->freeSprite(game->walls.Left);

	PADDLE_destroy(game->paddle);
	BALL_destroy(game->ball);

    pd_free(game);
    game = NULL;

    return 0;
}
