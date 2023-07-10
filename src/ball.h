#pragma once
#include "pd_api.h"

void ball_create(PlaydateAPI* playdate, LCDSprite* ball, int x, int y, LCDBitmap* image);
void ball_update_sprite(LCDSprite* sprite);

SpriteCollisionResponseType ball_collision_response(LCDSprite* sprite, LCDSprite* other);