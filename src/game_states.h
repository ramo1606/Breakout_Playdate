#pragma once
#include "pd_api.h"
#include "resource_manager.h"

typedef struct Logo Logo;
typedef struct Start Start;
typedef struct Game Game;
typedef struct GameOver GameOver;
typedef struct GameOverWait GameOverWait;
typedef struct LevelOver LevelOver;
typedef struct LevelOverWait LevelOverWait;
typedef struct Win Win;
typedef struct WinWait WinWait;

// Create functions
// Update functions
Logo* logo_create(Resources* resources);
Start* start_create(Resources* resources);
Game* game_create(Resources* resources);
GameOver* game_over_create(Resources* resources);
GameOverWait* game_over_wait_create(Resources* resources);
LevelOver* level_over_create(Resources* resources);
LevelOverWait* level_over_wait_create(Resources* resources);
Win* win_create(Resources* resources);
WinWait* win_wait_create(Resources* resources);

// Update functions
void logo_update(Logo* logo);
void start_update(Start* start);
void game_update(Game* game);
void game_over_update(GameOver* game_over);
void game_over_wait_update(GameOverWait* game_over_wait);
void level_over_update(LevelOver* level_over);
void level_over_wait_update(LevelOverWait* level_over_wait);
void win_update(Win* win);
void win_wait_update(WinWait* win_wait);

// Draw functions
void logo_draw(Logo* logo);
void start_draw(Start* start);
void game_draw(Game* game);
void game_over_draw(GameOver* game_over);
void game_over_wait_draw(GameOverWait* game_over_wait);
void level_over_draw(LevelOver* level_over);
void level_over_wait_draw(LevelOverWait* level_over_wait);
void win_draw(Win* win);
void win_wait_draw(WinWait* win_wait);