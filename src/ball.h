#pragma once
#include "pd_api.h"
#include <stdbool.h>

typedef struct BallData BallData;

LCDSprite* ball_create(float x, float y, LCDBitmap* image);
void ball_destroy(LCDSprite* sprite);
void ball_update_sprite(LCDSprite* sprite);

void ball_set_dx(LCDSprite* sprite, float value);
float ball_get_dx(LCDSprite* sprite);
void ball_set_dy(LCDSprite* sprite, float value);
float ball_get_dy(LCDSprite* sprite);
void ball_set_stuck(LCDSprite* sprite, bool stuck);
bool ball_is_stuck(LCDSprite* sprite);
bool ball_is_dead(LCDSprite* sprite);

SpriteCollisionResponseType ball_collision_response(LCDSprite* sprite, LCDSprite* other);