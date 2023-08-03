#include "paddle.h"
#include "common.h"
#include "ball.h"
#include "memory.h"
#include "utils.h"

struct PaddleData
{
	float dx;
};

LCDSprite* PADDLE_create(float x, float y, LCDBitmap* image)
{
	PlaydateAPI* pd = getPlaydateAPI();

	// Create new sprite
	LCDSprite* paddle = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(paddle, PADDLE_updateSprite);
	pd->sprite->setImage(paddle, image, kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(image, &w, &h, NULL, NULL, NULL);

	// Create collision rect for paddle
	PDRect cr = PDRectMake(0, 0, (float)w, (float)h);
	pd->sprite->setCollideRect(paddle, cr);
	pd->sprite->setCollisionResponseFunction(paddle, PADDLE_collisionResponse);

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

void PADDLE_destroy(LCDSprite* sprite)
{
	getPlaydateAPI()->sprite->removeSprite(sprite);
	PaddleData* paddle_data = (PaddleData*)getPlaydateAPI()->sprite->getUserdata(sprite);
	pd_free(paddle_data);
	pd_free(sprite);
}

void PADDLE_updateSprite(LCDSprite* sprite)
{
	if (sprite)
	{
		PlaydateAPI* pd = getPlaydateAPI();
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
					if (BALL_isStuck(ball)) 
					{
						//pd->sprite->moveBy(sprite, paddle_data->dx, 0.f);
						pd->sprite->moveTo(sprite, mid((pad_rect.width / 2), pad_x + paddle_data->dx, (pd->display->getWidth() - (pad_rect.width / 2))), pad_y);
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
						if (sign(BALL_getDx(ball)) == sign(PADDLE_getDx(sprite)))
						{
							BALL_setDx(ball, BALL_getDx(ball) + PADDLE_getDx(sprite));
						}
						else
						{
							BALL_setDx(ball, -BALL_getDx(ball));
							BALL_setDx(ball, BALL_getDx(ball) + PADDLE_getDx(sprite));
						}

						if (ball_x < pad_x)
						{
							pd->sprite->moveTo(ball, pad_x - (pad_rect.width / 2), pad_y);
						}
						else
						{
							pd->sprite->moveTo(ball, pad_x + (pad_rect.width / 2), pad_y);
						}

						if (!BALL_rammed(ball))
						{
							BALL_setRammed(ball, true);
						}
					}
				}
			}
			pd->sprite->moveTo(sprite, mid((pad_rect.width / 2), pad_x + paddle_data->dx, (pd->display->getWidth() - (pad_rect.width / 2))), pad_y);
		}
	}
}

SpriteCollisionResponseType PADDLE_collisionResponse(LCDSprite* sprite, LCDSprite* other)
{
	return kCollisionTypeOverlap;
}

void PADDLE_setDx(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		PaddleData* paddle_data = (PaddleData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (paddle_data)
		{
			paddle_data->dx = value;
		}
	}
}

float PADDLE_getDx(LCDSprite* sprite)
{
	if(sprite)
	{
		PaddleData* paddle_data = (PaddleData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (paddle_data)
		{
			return paddle_data->dx;
		}
	}
}
