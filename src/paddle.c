#include "paddle.h"
#include "common.h"
#include "ball.h"
#include "memory.h"
#include "utils.h"

struct PaddleData
{
	float dx;
};

LCDSprite* paddle_create(float x, float y, LCDBitmap* image)
{
	PlaydateAPI* pd = get_playdate_API();

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
	PaddleData* paddle_data = (PaddleData*)pd_malloc(sizeof(PaddleData));
	paddle_data->dx = 0.f;
	pd->sprite->setUserdata(paddle, (void*)paddle_data);

	return paddle;
}

void paddle_destroy(LCDSprite* sprite)
{
	PaddleData* paddle_data = (PaddleData*)get_playdate_API()->sprite->getUserdata(sprite);
	pd_free(paddle_data);
	pd_free(sprite);
}

void paddle_update_sprite(LCDSprite* sprite)
{
	if (sprite)
	{
		PlaydateAPI* pd = get_playdate_API();
		PaddleData* paddle_data = (PaddleData*)pd->sprite->getUserdata(sprite);
		if (paddle_data)
		{
			//pd->sprite->moveBy(sprite, paddle_data->dx, 0.f);
			
			float pad_x = 0;
			float pad_y = 0;
			pd->sprite->getPosition(sprite, &pad_x, &pad_y);

			PDRect pad_rect = pd->sprite->getCollideRect(sprite);

			float actual_x = 0;
			float actual_y = 0;
			int len = 0;
			SpriteCollisionInfo* collisions = NULL;
			collisions = pd->sprite->moveWithCollisions(sprite, pad_x + paddle_data->dx, pad_y, &actual_x, &actual_y, &len);

			if (len != 0) 
			{
				if (pd->sprite->getTag(collisions[0].other) == BALL) 
				{
					LCDSprite* ball = collisions[0].other;
					if (ball_is_stuck(ball)) 
					{
						//pd->sprite->moveBy(sprite, paddle_data->dx, 0.f);
						pd->sprite->moveTo(sprite, middleOfThree((pad_rect.width / 2), pad_x + paddle_data->dx, (pd->display->getWidth() - (pad_rect.width / 2))), pad_y);
					}

					if (collisions[0].normal.y = -1) 
					{
						return;
					}

					float ball_x = 0;
					float ball_y = 0;
					pd->sprite->getPosition(ball, &ball_x, &ball_y);
					PDRect ball_rect = pd->sprite->getCollideRect(ball);
					if (ball_y > pad_y - (pad_rect.height / 2) - 3) 
					{
						if (sign(ball_get_dx(ball)) == sign(paddle_get_dx(sprite)))
                                                {
                                                        ball_set_dx(ball, ball_get_dx(ball) + paddle_get_dx(sprite));
                                                }
                                                else
                                                {
                                                        ball_set_dx(ball, -ball_get_dx(ball));
                                                        ball_set_dx(ball, ball_get_dx(ball) + paddle_get_dx(sprite));
                                                }
 
                                                if (ball_x < pad_x)
                                                {
                                                        pd->sprite->moveTo(ball, pad_x - (pad_rect.width / 2), pad_y);
                                                }
                                                else
                                                {
                                                        pd->sprite->moveTo(ball, pad_x + (pad_rect.width / 2), pad_y);
                                                }

						if (!ball_rammed(ball))
                                                {
                                                        ball_set_rammed(ball, true);
                                                }
					}
				}
			}
			pd->sprite->moveTo(sprite, middleOfThree((pad_rect.width / 2), pad_x + paddle_data->dx, (pd->display->getWidth() - (pad_rect.width / 2))), pad_y);
		}
	}
}

SpriteCollisionResponseType paddle_collision_response(LCDSprite* sprite, LCDSprite* other)
{
	return kCollisionTypeOverlap;
}

void paddle_set_dx(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		PaddleData* paddle_data = (PaddleData*)get_playdate_API()->sprite->getUserdata(sprite);
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
		PaddleData* paddle_data = (PaddleData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (paddle_data)
		{
			return paddle_data->dx;
		}
	}
}
