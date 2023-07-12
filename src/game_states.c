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

struct Game 
{
	//Effects

	// Game objects
	LCDSprite* paddle;
	LCDSprite* ball;

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

Game* game_create(Resources* resources)
{
	Game* game = pd_malloc(sizeof(Game));
	game->paddle = paddle_create(200.f, 230.f, get_image(resources, "paddle"));
	game->ball = ball_create(100.f, 100.f, get_image(resources, "ball"));
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
