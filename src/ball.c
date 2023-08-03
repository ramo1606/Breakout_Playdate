#include "ball.h"

#include "common.h"
#include "utils.h"
#include "memory.h"
#include "resourcemanager.h"

#include "brick.h"
#include "paddle.h"

#include <stdlib.h>

static const float BALL_DX = 1.f;
static const float BALL_DY = 1.f;
static const float BALL_SPEED = 2.5f;
static const float BALL_ANGLE = 1.f;

struct BallData
{
	float dx;
	float dy;
	float speed;
	float angle;
	bool stuck;
	bool dead;
	bool rammed;
	int collisionCount;
	ESpriteType lastCollision;
};

LCDSprite* BALL_create(float x, float y)
{
	PlaydateAPI* pd = getPlaydateAPI();

	LCDSprite* ball = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(ball, BALL_updateSprite);
	pd->sprite->setImage(ball, RESOURCEMANAGER_getImage("ball"), kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(RESOURCEMANAGER_getImage("ball"), &w, &h, NULL, NULL, NULL);

	// Create collision rect for ball
	PDRect cr = PDRectMake(0, 0, (float)w, (float)h);
	pd->sprite->setCollideRect(ball, cr);
	pd->sprite->setCollisionResponseFunction(ball, BALL_collisionResponse);

	pd->sprite->moveTo(ball, x, y);
	pd->sprite->setZIndex(ball, 1000);
	pd->sprite->addSprite(ball);

	pd->sprite->setTag(ball, BALL);

	// Initialize paddle data
	BallData* ballData = pd_malloc(sizeof(BallData));
	ballData->dx = BALL_DX;
	ballData->dy = BALL_DY;
	ballData->speed = BALL_SPEED;
	ballData->angle = BALL_ANGLE;
	ballData->stuck = false;
	ballData->dead = false;
	ballData->rammed = false;
	ballData->collisionCount = 0;
	ballData->lastCollision = NONE;
	pd->sprite->setUserdata(ball, (void*)ballData);
	return ball;
}

void BALL_destroy(LCDSprite* sprite)
{
	getPlaydateAPI()->sprite->removeSprite(sprite);
	BallData* ball_data = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
	pd_free(ball_data);
	pd_free(sprite);
}

void BALL_reflect(BallData* ball_data, CollisionVector normal)
{
	if (normal.x != 0)
	{
		ball_data->dx = -ball_data->dx;
	}
	if (normal.y != 0)
	{
		ball_data->dy = -ball_data->dy;
	}
}

void BALL_updateSprite(LCDSprite* sprite)
{
	if (sprite)
	{
		PlaydateAPI* pd = getPlaydateAPI();
		BallData* ball_data = (BallData*)pd->sprite->getUserdata(sprite);
		if (ball_data && !BALL_isDead(sprite) && !BALL_isStuck(sprite))
		{
			float x = 0;
			float y = 0;
			pd->sprite->getPosition(sprite, &x, &y);

			float nextx = x + ball_data->dx * ball_data->speed;
			float nexty = y + ball_data->dy * ball_data->speed;

			PDRect ball_rect = pd->sprite->getCollideRect(sprite);

			int len = 0;
			float actual_x = 0;
			float actual_y = 0;
			SpriteCollisionInfo* collisions = NULL;
			collisions = pd->sprite->moveWithCollisions(sprite, nextx, nexty, &actual_x, &actual_y, &len);

			if(len == 1)
			{
				pd->sprite->moveTo(sprite, actual_x, actual_y);
				uint8_t other_tag = pd->sprite->getTag(collisions[0].other);

				if (other_tag == WALL) 
				{
					if (collisions[0].normal.y == -1) 
					{
						ball_data->dead = true;
					}
					else 
					{
						BALL_reflect(ball_data, collisions[0].normal);
					}
				}

				if (other_tag == PADDLE) 
				{
					float pad_x = 0;
					float pad_y = 0;
					pd->sprite->getPosition(collisions[0].other, &pad_x, &pad_y);

					bool bend = false;
					bool angf = false;
					if (collisions[0].normal.x != 0)
					{
						if (actual_y + ball_rect.width > pad_y + 3)
						{
							ball_data->rammed = true;
						}
						else
						{
							bend = true;
							angf = false;
							collisions[0].normal.y = -1;
							pd->sprite->moveTo(sprite, nextx, pad_y - ball_rect.height);
						}
					}

					BALL_reflect(ball_data, collisions[0].normal);

					if (collisions[0].normal.y == -1)
					{
						if (!bend && abs(PADDLE_getDx(collisions[0].other)) > 1)
						{
							bend = true;
							if (sign(PADDLE_getDx(collisions[0].other)) == sign(BALL_getDx(sprite)))
							{
								angf = true;
							}
							else
							{
								angf = false;
							}
						}
						
						if (bend)
                        {
							if (angf)
							{
								BALL_setAngle(sprite, mid(0, ball_data->angle - 1, 2));
							}
							else
							{
								if (ball_data->angle == 2)
								{
									ball_data->dx = -ball_data->dx;
								}
								else
								{
									BALL_setAngle(sprite, mid(0, ball_data->angle + 1, 2));
								}
							}
						}
                    }
				}

				if (other_tag == BRICK) 
				{
					pd->sprite->removeSprite(collisions[0].other);
					BRICK_destroy(collisions[0].other);
					BALL_reflect(ball_data, collisions[0].normal);
				}
			}
		}
	}
}

void BALL_setDx(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			ball_data->dx = value;
		}
	}
}

float BALL_getDx(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			return ball_data->dx;
		}
	}
	return 0;
}

void BALL_setDy(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			ball_data->dy = value;
		}
	}
}

float BALL_getDy(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			return ball_data->dy;
		}
	}
	return 0;
}

void BALL_setStuck(LCDSprite* sprite, bool stuck)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			ball_data->stuck = stuck;
		}
	}
}

bool BALL_isStuck(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			return ball_data->stuck;
		}
	}
	return false;
}

bool BALL_isDead(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			return ball_data->dead;
		}
	}
	return false;
}

SpriteCollisionResponseType BALL_collisionResponse(LCDSprite* sprite, LCDSprite* other)
{
	return kCollisionTypeBounce;
}

void BALL_setAngle(LCDSprite* sprite, float angle)
{
	if (sprite)
    {
		BallData* ball_data = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			ball_data->angle = angle;
			if (areEqual(angle, 2.f))
			{
				ball_data->dx = .5f * sign(ball_data->dx);
				ball_data->dy = 1.3f * sign(ball_data->dy);
			}
			else if (areEqual(angle, 0.f))
			{
				ball_data->dx = 1.3f * sign(ball_data->dx);
				ball_data->dy = .5f * sign(ball_data->dy);
			}
			else
			{
				ball_data->dx = 1.f * sign(ball_data->dx);
				ball_data->dy = 1.f * sign(ball_data->dy);
			}
		}
    }
}

bool BALL_rammed(LCDSprite* sprite)
{
	if (sprite)
    {
		BallData* ball_data = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			return ball_data->rammed;
        }
    }
	return false;
}

void BALL_setRammed(LCDSprite* sprite, bool rammed)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
        if (ball_data)
        {
			ball_data->rammed = rammed;
		}
	}
}
