#include "paddle.h"
#include "enums.h"

static PlaydateAPI* pd = NULL;

static const int DX = 6;
static int current_dx = 0;

void paddle_create(PlaydateAPI* playdate, LCDSprite* paddle, int x, int y, LCDBitmap* image)
{
	pd = playdate;

	paddle = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(paddle, paddle_update_sprite);
	pd->sprite->setImage(paddle, image, kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(image, &w, &h, NULL, NULL, NULL);

	PDRect cr = PDRectMake(0, 0, (float)w, (float)h);
	pd->sprite->setCollideRect(paddle, cr);
	pd->sprite->setCollisionResponseFunction(paddle, paddle_collision_response);

	pd->sprite->moveTo(paddle, (float)x, (float)y);
	pd->sprite->setZIndex(paddle, 1000);
	pd->sprite->addSprite(paddle);

	pd->sprite->setTag(paddle, PADDLE);
}

void paddle_update_sprite(LCDSprite* sprite)
{
	PDButtons current;
	pd->system->getButtonState(&current, NULL, NULL);

	bool button_pressed = false;

	if (current & kButtonLeft) 
	{
		current_dx = -DX;
		button_pressed = true;
	}
	else if (current & kButtonRight) 
	{
		current_dx = DX;
		button_pressed = true;
	}

	if (!button_pressed)
	{
		current_dx /= 1.3;
	}
	pd->sprite->moveBy(sprite, current_dx, 0);
}

SpriteCollisionResponseType paddle_collision_response(LCDSprite* sprite, LCDSprite* other)
{
}

void paddle_process_input(bool pressed, int direction)
{
	if (pressed) 
	{
		current_dx = direction * DX;
	}
	else 
	{
		current_dx /= 1.3;
	}
}
