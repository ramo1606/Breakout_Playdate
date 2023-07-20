#pragma once
#include "pd_api.h"

typedef struct BrickData BrickData;

LCDSprite* brick_create(float x, float y, LCDBitmap* image);
void brick_destroy(LCDSprite* sprite);
void brick_update_sprite(LCDSprite* sprite);

void brick_set_dx(LCDSprite* sprite, float value);
float brick_get_dx(LCDSprite* sprite);
void brick_set_dy(LCDSprite* sprite, float value);
float brick_get_dy(LCDSprite* sprite);

SpriteCollisionResponseType brick_collision_response(LCDSprite* sprite, LCDSprite* other);