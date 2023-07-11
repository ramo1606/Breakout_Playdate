#pragma once
#include "pd_api.h"
#include <stdbool.h>

typedef struct PaddleData PaddleData;

LCDSprite* paddle_create(PlaydateAPI* playdate, float x, float y, LCDBitmap* image);
void paddle_update_sprite(LCDSprite* sprite);
void paddle_set_dx(LCDSprite* sprite, float value);
float paddle_get_dx(LCDSprite* sprite);

SpriteCollisionResponseType paddle_collision_response(LCDSprite* sprite, LCDSprite* other);