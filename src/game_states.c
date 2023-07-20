#include "game_states.h"
#include "memory.h"
#include "common.h"
#include "paddle.h"
#include "ball.h"

#include <stdbool.h>

struct Logo 
{
	int logo;
};

struct Start 
{
	int start;
};

typedef struct 
{
	LCDSprite* Top;
	LCDSprite* Bottom;
	LCDSprite* Right;
	LCDSprite* Left;
}Walls;

struct Game 
{
	//Effects

	// Game objects
	LCDSprite* paddle;
	LCDSprite* ball;

	Walls walls;

	//VFX

	//Levels
	char* current_level;
};

struct GameOver 
{
	int game_over;
};

struct GameOverWait
{
	int game_over_wait;
};

struct LevelOver
{
	int level_over;
};

struct LevelOverWait
{
	int level_over_wait;
};

struct Win
{
	int win;
};

struct WinWait
{
	int win_wait;
};

void logo_update(Logo* logo)
{
}

void start_update(Start* start)
{
}

void game_update(Game* game)
{
	PDButtons current;
	get_playdate_API()->system->getButtonState(&current, NULL, NULL);
	bool button_pressed = false;

	if (current & kButtonLeft)
	{
		paddle_set_dx(game->paddle, -PADDLE_DX);
		button_pressed = true;
	}
	else if (current & kButtonRight)
	{
		paddle_set_dx(game->paddle, PADDLE_DX);
		button_pressed = true;
	}

	if (!button_pressed)
	{
		paddle_set_dx(game->paddle, paddle_get_dx(game->paddle) / 1.3f);
	}

	get_playdate_API()->sprite->updateAndDrawSprites();
}

void game_over_update(GameOver* game_over)
{
}

void game_over_wait_update(GameOverWait* game_over_wait)
{
}

void level_over_update(LevelOver* level_over)
{
}

void level_over_wait_update(LevelOverWait* level_over_wait)
{
}

void win_update(Win* win)
{
}

void win_wait_update(WinWait* win_wait)
{
}

void logo_draw(Logo* logo)
{
	int x = (400 - TEXT_WIDTH) / 2;
	int y = (240 - TEXT_HEIGHT) / 2;
	get_playdate_API()->graphics->setFont(font);
	get_playdate_API()->graphics->drawText("LOGO!", strlen("LOGO!"), kASCIIEncoding, x, y);
}

void start_draw(Start* start)
{
	int x = (400 - TEXT_WIDTH) / 2;
	int y = (240 - TEXT_HEIGHT) / 2;
	get_playdate_API()->graphics->setFont(font);
	get_playdate_API()->graphics->drawText("START!", strlen("START!"), kASCIIEncoding, x, y);
}

void game_draw(Game* game)
{
}

void game_over_draw(GameOver* game_over)
{
	int x = (400 - TEXT_WIDTH) / 2;
	int y = (240 - TEXT_HEIGHT) / 2;
	get_playdate_API()->graphics->setFont(font);
	get_playdate_API()->graphics->drawText("GAME OVER!", strlen("GAME OVER!"), kASCIIEncoding, x, y);
}

void game_over_wait_draw(GameOverWait* game_over_wait)
{
}

void level_over_draw(LevelOver* level_over)
{
}

void level_over_wait_draw(LevelOverWait* level_over_wait)
{
}

void win_draw(Win* win)
{
}

void win_wait_draw(WinWait* win_wait)
{
}

Logo* logo_create(Resources* resources)
{
	return NULL;
}

Start* start_create(Resources* resources)
{
	return NULL;
}

void create_walls(Game* game) 
{
	// I will use invisible sprites with collisions to check the collision with the border of the screens, 
	// this way I don't have to deal with screen collision and take advantage of Playdate collision system
	
	// Create Walls 
	PlaydateAPI* pd = get_playdate_API();
	game->walls.Top = pd->sprite->newSprite();
	game->walls.Bottom = pd->sprite->newSprite();
	game->walls.Right = pd->sprite->newSprite();
	game->walls.Left = pd->sprite->newSprite();

	int screen_width = pd->display->getWidth();
	int screen_height = pd->display->getHeight();

	// Top Wall
	PDRect top_collision_rect = PDRectMake(0, 0, (float)screen_width, 1.f);
	pd->sprite->setCollideRect(game->walls.Top, top_collision_rect);
	pd->sprite->moveTo(game->walls.Top, 0.f, -1.f);
	pd->sprite->setTag(game->walls.Top, WALL);
	pd->sprite->setZIndex(game->walls.Top, 1000);
	pd->sprite->addSprite(game->walls.Top);

	// `Bottom Wall
	PDRect bottom_collision_rect = PDRectMake(0.f, 0.f, (float)screen_width, 1.f);
	pd->sprite->setCollideRect(game->walls.Bottom, bottom_collision_rect);
	pd->sprite->moveTo(game->walls.Bottom, 0.f, screen_height);
	pd->sprite->setTag(game->walls.Bottom, WALL);
	pd->sprite->setZIndex(game->walls.Bottom, 1000);
	pd->sprite->addSprite(game->walls.Bottom);

	// Right Wall
	PDRect right_collision_rect = PDRectMake(0.f, 0.f, 1.f, (float)screen_height);
	pd->sprite->setCollideRect(game->walls.Right, right_collision_rect);
	pd->sprite->moveTo(game->walls.Right, screen_width, 0.f);
	pd->sprite->setTag(game->walls.Right, WALL);
	pd->sprite->setZIndex(game->walls.Right, 1000);
	pd->sprite->addSprite(game->walls.Right);

	//Left Wall
	PDRect left_collision_rect = PDRectMake(0.f, 0.f, 1.f, (float)screen_height);
	pd->sprite->setCollideRect(game->walls.Left, left_collision_rect);
	pd->sprite->moveTo(game->walls.Left, -1.f, 0.f);
	pd->sprite->setTag(game->walls.Left, WALL);
	pd->sprite->setZIndex(game->walls.Left, 1000);
	pd->sprite->addSprite(game->walls.Left);
}

Game* game_create(Resources* resources)
{
	Game* game = pd_malloc(sizeof(Game));
	game->paddle = paddle_create(200.f, 230.f, get_image(resources, "paddle"));
	game->ball = ball_create(100.f, 100.f, get_image(resources, "ball"));

	create_walls(game);

	return game;
}

void game_destroy(Game* game) 
{
	PlaydateAPI* pd = get_playdate_API();
	pd->sprite->freeSprite(game->walls.Top);
	pd->sprite->freeSprite(game->walls.Bottom);
	pd->sprite->freeSprite(game->walls.Right);
	pd->sprite->freeSprite(game->walls.Left);

	paddle_destroy(game->paddle);
	ball_destroy(game->ball);
}

GameOver* game_over_create(Resources* resources)
{
}

GameOverWait* game_over_wait_create(Resources* resources)
{
}

LevelOver* level_over_create(Resources* resources)
{
}

LevelOverWait* level_over_wait_create(Resources* resources)
{
}

Win* win_create(Resources* resources)
{
}

WinWait* win_wait_create(Resources* resources)
{
}