#include "game.h"
#include "memory.h"
#include "resource_manager.h"
#include "common.h"
#include "game_states.h"

#include <stdlib.h>
#include <math.h>

typedef struct Breakout
{
	//Resources
	Resources* resources;

	//Mode
	EMode mode;

	Logo* logo_state;
	Start* start_state;
	Game* game_state;
	GameOver* game_over_state;
}Breakout;

static Breakout breakout;

void create_game(PlaydateAPI* p)
{
	initializeMemoryTools(p);

	//API
	set_playdate_API(p);

	breakout.resources = load_resources();

	//Fonts
	const char* err;
	font = get_playdate_API()->graphics->loadFont(fontpath, &err);

	if (font == NULL)
		get_playdate_API()->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

	//Mode
	breakout.mode = LOGO;

	// Game objects
	breakout.logo_state = logo_create(breakout.resources);
	breakout.start_state = start_create(breakout.resources);
	breakout.game_state = game_create(breakout.resources);
	breakout.game_over_state = game_over_create(breakout.resources);

	//VFX

	//Levels
	//breakout.current_level = "";
}

void process_input()
{
	PDButtons pushed;
	get_playdate_API()->system->getButtonState(NULL, &pushed, NULL);

	switch (breakout.mode)
	{
	case LOGO:
		if (pushed & kButtonA)
		{
			breakout.mode = START;
		}
		break;
	case START:
		if (pushed & kButtonA)
		{
			breakout.mode = GAME;
		}
		break;
	case GAME:
		if (pushed & kButtonA)
		{
			breakout.mode = GAME_OVER;
		}
		break;
	case GAME_OVER:
		if (pushed & kButtonA)
		{
			breakout.mode = START;
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

	get_playdate_API()->graphics->clear(kColorWhite);
	switch (breakout.mode)
	{
	case LOGO:
		logo_update(breakout.logo_state);
		logo_draw(breakout.logo_state);
		break;
	case START:
		start_update(breakout.start_state);
		start_draw(breakout.start_state);
		break;
	case GAME:
		game_update(breakout.game_state);
		game_draw(breakout.game_state);
		break;
	case GAME_OVER:
		game_over_draw(breakout.game_over_state);
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
