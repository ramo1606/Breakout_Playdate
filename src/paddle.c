#include "paddle.h"
#include "common.h"
#include "memory.h"

static PlaydateAPI* pd = NULL;

struct PaddleData
{
	float dx;
};

LCDSprite* paddle_create(PlaydateAPI* playdate, float x, float y, LCDBitmap* image)
{
	if(!pd)
	{
		pd = playdate;
	}

	// Create new sprite
	LCDSprite* paddle = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(paddle, paddle_update_sprite);
	pd->sprite->setImage(paddle, image, kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(image, &w, &h, NULL, NULL, NULL);

	// Create collision rect for paddle
	PDRect cr = PDRectMake(0, 0, (float)w, (float)h);
	pd->sprite->setCollideRect(paddle, cr);
	pd->sprite->setCollisionResponseFunction(paddle, paddle_collision_response);

	pd->sprite->moveTo(paddle, x, y);
	pd->sprite->setZIndex(paddle, 1000);
	pd->sprite->addSprite(paddle);

	pd->sprite->setTag(paddle, PADDLE);

	// Initialize paddle data
	PaddleData* paddle_data = pd_malloc(sizeof(PaddleData));
	paddle_data->dx = 0.f;
	pd->sprite->setUserdata(paddle, (void*)paddle_data);

	return paddle;
}

void paddle_update_sprite(LCDSprite* sprite)
{
	if (sprite)
	{
		PaddleData* paddle_data = (PaddleData*)pd->sprite->getUserdata(sprite);
		if (paddle_data)
		{
			pd->sprite->moveBy(sprite, paddle_data->dx, 0.f);
		}
	}
}

SpriteCollisionResponseType paddle_collision_response(LCDSprite* sprite, LCDSprite* other)
{
	return kCollisionTypeSlide;
}

void paddle_set_dx(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		PaddleData* paddle_data = (PaddleData*)pd->sprite->getUserdata(sprite);
		if (paddle_data)
		{
			paddle_data->dx = value;
		}
	}
}

float paddle_get_dx(LCDSprite* sprite)
{
	if(sprite)
	{
		PaddleData* paddle_data = (PaddleData*)pd->sprite->getUserdata(sprite);
		if (paddle_data)
		{
			return paddle_data->dx;
		}
	}
}