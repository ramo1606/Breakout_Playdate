#include "game_states.h"
#include "memory.h"
#include "common.h"
#include "paddle.h"
#include "ball.h"
#include "brick.h"
#include "level.h"

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

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
	// Resources
	Resources* resources;

	//Effects

	// Game objects
	LCDSprite* paddle;
	LCDSprite* ball;
	LCDSprite** bricks;

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

void serve_ball(Game* game)
{
	PlaydateAPI* pd = get_playdate_API();

	float paddle_x = 0;
	float paddle_y = 0;
	pd->sprite->getPosition(game->paddle, &paddle_x, &paddle_y);
	PDRect paddle_rect = pd->sprite->getCollideRect(game->paddle);

	game->ball = ball_create(paddle_x, paddle_y - (paddle_rect.width / 2), get_image(game->resources, "ball"));

	//pd->sprite->moveTo(game->ball, paddle_x, paddle_y - (paddle_rect.width / 2));
	ball_set_stuck(game->ball, true);
}

void release_stuck(Game* game) 
{
	PlaydateAPI* pd = get_playdate_API();

	if (ball_is_stuck(game->ball))
	{
		float x = 0;
		float y = 0;
		pd->sprite->getPosition(game->ball, &x, &y);
		pd->sprite->moveTo(game->ball, x, y);
		ball_set_stuck(game->ball, false);
	}
}

void logo_update(Logo* logo)
{
}

void start_update(Start* start)
{
}

void game_update(Game* game)
{
	PlaydateAPI* pd = get_playdate_API();
	PDButtons current;
	pd->system->getButtonState(&current, NULL, NULL);
	bool button_pressed = false;

	if (current & kButtonLeft)
	{
		paddle_set_dx(game->paddle, -PADDLE_DX);
		button_pressed = true;
	}
	if (current & kButtonRight)
	{
		paddle_set_dx(game->paddle, PADDLE_DX);
		button_pressed = true;
	}
	if (current & kButtonA) 
	{
		release_stuck(game);
	}
	
	if (!button_pressed)
	{
		paddle_set_dx(game->paddle, paddle_get_dx(game->paddle) / 1.3f);
	}

	if (ball_is_dead(game->ball))
	{
		ball_destroy(game->ball);
		serve_ball(game);
	}

	get_playdate_API()->sprite->updateAndDrawSprites();

	if (ball_is_stuck(game->ball))
	{
		float pad_x = 0.f;
		float pad_y = 0.f;
		pd->sprite->getPosition(game->paddle, &pad_x, &pad_y);

		PDRect ball_rect = pd->sprite->getCollideRect(game->ball);
		get_playdate_API()->sprite->moveTo(game->ball, pad_x, pad_y - ball_rect.height);
	}
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
	pd->sprite->moveTo(game->walls.Bottom, 0.f, (float)screen_height);
	pd->sprite->setTag(game->walls.Bottom, WALL);
	pd->sprite->setZIndex(game->walls.Bottom, 1000);
	pd->sprite->addSprite(game->walls.Bottom);

	// Right Wall
	PDRect right_collision_rect = PDRectMake(0.f, 0.f, 1.f, (float)screen_height);
	pd->sprite->setCollideRect(game->walls.Right, right_collision_rect);
	pd->sprite->moveTo(game->walls.Right, (float)screen_width, 0.f);
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

void load_level_bricks(Game* game, Resources* resources)
{
	char* current_level = level_get(0);
	game->bricks = pd_malloc(sizeof(LCDSprite*) * strlen(current_level));

	// b = normal brick
	// x = empty space
	// i = indestructable brick
	// h = hardened brick
	// s = sploding brick
	// p = powerup brickf

	char current_brick = '\0';
	char last_brick = '\0';
	int z = -1;
	for (int index = 0; index < strlen(current_level); ++index)
	{
		z += 1;
		current_brick = current_level[index];
		if (current_brick == 'i' ||
			current_brick == 'b' ||
			current_brick == 'h' ||
			current_brick == 's' ||
			current_brick == 'p') 
		{
			int w, h;
			get_playdate_API()->graphics->getBitmapData(get_image(resources, "brick"), &w, &h, NULL, NULL, NULL);
			float x = 80 + ((z - 1) % 11) * (w + 6);
			float y = 10 + (int)floor((z - 1) / 11) * (h + 4);
			game->bricks[index] = brick_create(x, y, get_image(resources, "brick"));
			last_brick = current_brick;
		}
		else if (current_brick == 'x') 
		{
			last_brick = current_brick;
		}
		else if (current_brick == '/') 
		{
			z = (int)floor(((z - 1) / 11) + 1) * 11;
		}
		else if (current_brick >= '0' && current_brick <= '9')
		{
			int tmp = atoi(&current_brick);
			for (int j = 0; j < tmp; ++j)
			{
				if (last_brick == 'i' ||
					last_brick == 'b' ||
					last_brick == 'h' ||
					last_brick == 's' ||
					last_brick == 'p') 
				{
					int w, h;
					get_playdate_API()->graphics->getBitmapData(get_image(resources, "brick"), &w, &h, NULL, NULL, NULL);
					float x = 80 + ((z - 1) % 11) * (w + 6);
					float y = 10 + floor((z - 1) / 11) * (h + 4);
					game->bricks[index] = brick_create(x, y, get_image(resources, "brick"));
				}
				z += 1;
			}
			z -= 1;
		}
	}
}

Game* game_create(Resources* resources)
{
	Game* game = pd_malloc(sizeof(Game));
	game->resources = resources;
	game->paddle = paddle_create(200.f, 230.f, get_image(resources, "paddle"));
	serve_ball(game);

	create_walls(game);
	load_level_bricks(game, resources);

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