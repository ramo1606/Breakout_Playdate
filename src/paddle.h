#pragma once
#include "pd_api.h"
#include <stdbool.h>

void paddle_create(PlaydateAPI* playdate, LCDSprite* paddle, int x, int y, LCDBitmap* image);
void paddle_update_sprite(LCDSprite* sprite);
void paddle_process_input(bool pressed, int direction);

SpriteCollisionResponseType paddle_collision_response(LCDSprite* sprite, LCDSprite* other);