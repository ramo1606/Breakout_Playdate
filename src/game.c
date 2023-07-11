#include "game.h"
#include "memory.h"
#include "resource_manager.h"
#include "ball.h"
#include "paddle.h"
#include "common.h"

#include <stdlib.h>
#include <math.h>

typedef struct Breakout
{
	// API
	PlaydateAPI* pd;

	//Font
	const char* fontpath;
	LCDFont* font;

	//Resources
	Resources* resources;

	//Mode
	EMode mode;

	//Effects

	// Game objects
	LCDSprite* paddle;
	LCDSprite* ball;

	//VFX

	//Levels
	char* current_level;
}Breakout;

static Breakout game;

void create_game(PlaydateAPI* p)
{
	initializeMemoryTools(p);

	//API
	game.pd = p;

	game.resources = load_resources(game.pd);

	//Fonts
	game.fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
	const char* err;
	game.font = game.pd->graphics->loadFont(game.fontpath, &err);

	if (game.font == NULL)
		game.pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, game.fontpath, err);

	//Mode
	game.mode = LOGO;

	//Effects

	// Game objects
	game.paddle = paddle_create(game.pd, 200.f, 230.f, get_image(game.resources, "paddle"));
	game.ball = ball_create(game.pd, 100.f, 100.f, get_image(game.resources, "ball"));

	//VFX

	//Levels
	game.current_level = "";
}

void update_logo()
{
}

void update_start()
{
}

void update_game()
{
	game.pd->sprite->updateAndDrawSprites();
}

void update_game_over()
{
}

void draw_logo()
{
	int x = (400 - TEXT_WIDTH) / 2;
	int y = (240 - TEXT_HEIGHT) / 2;
	game.pd->graphics->setFont(game.font);
	game.pd->graphics->drawText("LOGO!", strlen("LOGO!"), kASCIIEncoding, x, y);
}

void draw_start()
{
	int x = (400 - TEXT_WIDTH) / 2;
	int y = (240 - TEXT_HEIGHT) / 2;
	game.pd->graphics->setFont(game.font);
	game.pd->graphics->drawText("START!", strlen("START!"), kASCIIEncoding, x, y);
}

void draw_game()
{
}

void draw_game_over()
{
	int x = (400 - TEXT_WIDTH) / 2;
	int y = (240 - TEXT_HEIGHT) / 2;
	game.pd->graphics->setFont(game.font);
	game.pd->graphics->drawText("GAME OVER!", strlen("GAME OVER!"), kASCIIEncoding, x, y);
}

void process_input()
{
	PDButtons pushed;
	game.pd->system->getButtonState(NULL, &pushed, NULL);

	switch (game.mode)
	{
	case LOGO:
		if (pushed & kButtonA)
		{
			game.mode = START;
		}
		break;
	case START:
		if (pushed & kButtonA)
		{
			game.mode = GAME;
		}
		break;
	case GAME:
		if (pushed & kButtonA)
		{
			game.mode = GAME_OVER;
			break;
		}

		PDButtons current;
		game.pd->system->getButtonState(&current, NULL, NULL);
		bool button_pressed = false;
		
		if (current & kButtonLeft)
		{
			paddle_set_dx(game.paddle, -PADDLE_DX);
			button_pressed = true;
		}
		else if (current & kButtonRight)
		{
			paddle_set_dx(game.paddle, PADDLE_DX);
			button_pressed = true;
		}

		if (!button_pressed)
		{
			paddle_set_dx(game.paddle, paddle_get_dx(game.paddle)/1.3f);
		}
	case GAME_OVER:
		if (pushed & kButtonA)
		{
			game.mode = START;
		}
		break;
	case GAME_OVER_WAIT:
		break;
	case LEVEL_OVER:
		break;
	case LEVEL_OVER_WAIT:
		break;
	case WINNER:
		break;
	case WINNER_WAIT:
		break;
	default:
		break;
	}
}

int update()
{
	process_input();
	//doblink()
	//doshake()
	//updateparts()
	//update_sash()

	game.pd->graphics->clear(kColorWhite);
	switch (game.mode)
	{
	case LOGO:
		update_logo();
		draw_logo();
		break;
	case START:
		update_start();
		draw_start();
		break;
	case GAME:
		update_game();
		draw_game();
		break;
	case GAME_OVER:
		draw_game_over();
		break;
	case GAME_OVER_WAIT:
		break;
	case LEVEL_OVER:
		break;
	case LEVEL_OVER_WAIT:
		break;
	case WINNER:
		break;
	case WINNER_WAIT:
		break;
	default:
		break;
	}

	return 1;
}
