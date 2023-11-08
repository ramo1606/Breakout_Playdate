#include "paddle.h"

#include "common.h"
#include "memory.h"
#include "particles.h"
#include "resourcemanager.h"
#include "utils.h"

#include "ball.h"

struct PaddleData
{
	float dx;
	int timerReduce;
	int timerExpand;

	int speedWind;
};

LCDSprite* PADDLE_create(float x, float y)
{
	PlaydateAPI* pd = getPlaydateAPI();

	// Create new sprite
	LCDSprite* paddle = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(paddle, PADDLE_updateSprite);
	pd->sprite->setImage(paddle, RESOURCEMANAGER_getImage("paddle"), kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(RESOURCEMANAGER_getImage("paddle"), &w, &h, NULL, NULL, NULL);

	// Create bounds rect for paddle
	PDRect bounds = PDRectMake(0.f, 0.f, (float)w, (float)h);
	pd->sprite->setBounds(paddle, bounds);

	// Create collision rect for paddle
	PDRect cr = PDRectMake(1.f, 1.f, (float)(w - 2), (float)(h - 2));
	pd->sprite->setCollideRect(paddle, cr);
	pd->sprite->setCollisionResponseFunction(paddle, PADDLE_collisionResponse);

	pd->sprite->moveTo(paddle, x, y);
	pd->sprite->setZIndex(paddle, 1000);
	pd->sprite->addSprite(paddle);

	pd->sprite->setTag(paddle, PADDLE);

	// Initialize paddle data
	PaddleData* paddle_data = pd_malloc(sizeof(PaddleData));
	paddle_data->dx = 0.f;
	paddle_data->speedWind = 0;
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
			if (paddle_data->timerExpand > 0) 
			{
				// TODO: expand pad
			}
			else if (paddle_data->timerReduce > 0)
			{
				//TODO: reduce pad
			}
			else 
			{
				//TODO: original size pad
			}
			
			float pad_x = 0;
			float pad_y = 0;
			pd->sprite->getPosition(sprite, &pad_x, &pad_y);

			PDRect pad_rect = pd->sprite->getCollideRect(sprite);

			float actual_x = 0;
			float actual_y = 0;
			int len = 0;
			//SpriteCollisionInfo* collisions = NULL;
			//collisions = pd->sprite->moveWithCollisions(sprite, pad_x + paddle_data->dx, pad_y, &actual_x, &actual_y, &len);
			float oldPad_x = pad_x;
			pd->sprite->moveTo(sprite, mid((pad_rect.width * 0.5f), pad_x + paddle_data->dx, (pd->display->getWidth() - (pad_rect.width * 0.5f))), pad_y);

			pd->sprite->getPosition(sprite, &pad_x, &pad_y);

			//if(!areEqual(pad_x, oldPad_x))
			//{
			//	paddle_data->speedWind = 0;
			//}

			if(paddle_data->speedWind > 5)
			{
				if(paddle_data->dx < 0.0f)
				{
					PARTICLES_spawnSpeedLines(sprite, pad_x + (pad_rect.width * 0.5f), pad_y);
				}
				else
				{
					PARTICLES_spawnSpeedLines(sprite, pad_x - (pad_rect.width * 0.5f), pad_y);
				}
			}
			//if (len != 0) 
			//{
			//	if (pd->sprite->getTag(collisions[0].other) == BALL) 
			//	{
			//		LCDSprite* ball = collisions[0].other;
			//		if (BALL_isStuck(ball)) 
			//		{
			//			//pd->sprite->moveBy(sprite, paddle_data->dx, 0.f);
			//			pd->sprite->moveTo(ball, mid((pad_rect.width / 2), pad_x + paddle_data->dx, (pd->display->getWidth() - (pad_rect.width / 2))), pad_y);
			//		}
			//
			//		if (collisions[0].normal.y == -1) 
			//		{
			//			return;
			//		}
			//		
			//		float ball_x = 0;
			//		float ball_y = 0;
			//		pd->sprite->getPosition(ball, &ball_x, &ball_y);
			//		if (ball_y > pad_y - (pad_rect.height / 2) - 3) 
			//		{
			//			if (sign(BALL_getDx(ball)) == sign(PADDLE_getDx(sprite)))
			//			{
			//				BALL_setDx(ball, BALL_getDx(ball) + PADDLE_getDx(sprite));
			//			}
			//			else
			//			{
			//				BALL_setDx(ball, -BALL_getDx(ball));
			//				BALL_setDx(ball, BALL_getDx(ball) + PADDLE_getDx(sprite));
			//			}
			//		
			//			if (ball_x < pad_x)
			//			{
			//				pd->sprite->moveTo(ball, pad_x - (pad_rect.width / 2), pad_y);
			//			}
			//			else
			//			{
			//				pd->sprite->moveTo(ball, pad_x + (pad_rect.width / 2), pad_y);
			//			}
			//		
			//			if (!BALL_rammed(ball))
			//			{
			//				BALL_setRammed(ball, true);
			//			}
			//		}
			//	}
			//}
			//pd->sprite->moveTo(sprite, mid((pad_rect.width / 2), pad_x + paddle_data->dx, (pd->display->getWidth() - (pad_rect.width / 2))), pad_y);
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
	return 0.f;
}

int PADDLE_getTimerExtend(LCDSprite* sprite)
{
	if (sprite)
	{
		PaddleData* paddle_data = (PaddleData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (paddle_data)
		{
			return paddle_data->timerExpand;
		}
	}
	return 0.f;
}

int PADDLE_getTimerReduce(LCDSprite* sprite)
{
	if (sprite)
	{
		PaddleData* paddle_data = (PaddleData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (paddle_data)
		{
			return paddle_data->timerReduce;
		}
	}
	return 0.f;
}

void PADDLE_setSpeedWind(LCDSprite* sprite, int value)
{
	if (sprite)
	{
		PaddleData* paddle_data = (PaddleData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (paddle_data)
		{
			paddle_data->speedWind = value;
		}
	}
}

int PADDLE_getSpeedWind(LCDSprite* sprite)
{
	if (sprite)
	{
		PaddleData* paddle_data = (PaddleData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (paddle_data)
		{
			return paddle_data->speedWind;
		}
	}
	return 0.f;
}
