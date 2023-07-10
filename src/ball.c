#include "ball.h"
#include "enums.h"

#include <stdbool.h>

static PlaydateAPI* pd = NULL;

static const int DX = 6;
static const int DY = 6;

static int current_dx = 0;
static int current_dy = 0;

void ball_create(PlaydateAPI* playdate, LCDSprite* ball, int x, int y, LCDBitmap* image)
{
	pd = playdate;

	ball = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(ball, ball_update_sprite);
	pd->sprite->setImage(ball, image, kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(image, &w, &h, NULL, NULL, NULL);

	PDRect cr = PDRectMake(0, 0, (float)w, (float)h);
	pd->sprite->setCollideRect(ball, cr);
	pd->sprite->setCollisionResponseFunction(ball, ball_collision_response);

	pd->sprite->moveTo(ball, (float)x, (float)y);
	pd->sprite->setZIndex(ball, 1000);
	pd->sprite->addSprite(ball);

	pd->sprite->setTag(ball, BALL);
}

void ball_update_sprite(LCDSprite* sprite)
{
}

SpriteCollisionResponseType ball_collision_response(LCDSprite* sprite, LCDSprite* other)
{
}
