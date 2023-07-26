#include "ball.h"
#include "brick.h"
#include "paddle.h"
#include "memory.h"
#include "common.h"
#include "utils.h"

#include <stdlib.h>

struct BallData
{
	float dx;
	float dy;
	float speed;
	float angle;
	bool stuck;
	bool dead;
	bool rammed;
};

LCDSprite* ball_create(float x, float y, LCDBitmap* image)
{
	PlaydateAPI* pd = get_playdate_API();

	LCDSprite* ball = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(ball, ball_update_sprite);
	pd->sprite->setImage(ball, image, kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(image, &w, &h, NULL, NULL, NULL);

	// Create collision rect for ball
	PDRect cr = PDRectMake(0, 0, (float)w, (float)h);
	pd->sprite->setCollideRect(ball, cr);
	pd->sprite->setCollisionResponseFunction(ball, ball_collision_response);

	pd->sprite->moveTo(ball, x, y);
	pd->sprite->setZIndex(ball, 1000);
	pd->sprite->addSprite(ball);

	pd->sprite->setTag(ball, BALL);

	// Initialize paddle data
	BallData* ball_data = (BallData*)pd_malloc(sizeof(BallData));
	ball_data->dx = BALL_DX;
	ball_data->dy = BALL_DY;
	ball_data->speed = BALL_SPEED;
	ball_data->angle = BALL_ANGLE;
	ball_data->stuck = false;
	ball_data->dead = false;
	ball_data->rammed = false;
	pd->sprite->setUserdata(ball, (void*)ball_data);
	return ball;
}

void ball_destroy(LCDSprite* sprite)
{
	get_playdate_API()->sprite->removeSprite(sprite);
	BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
	pd_free(ball_data);
	pd_free(sprite);
}

void ball_reflect(BallData* ball_data, CollisionVector normal)
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

void ball_update_sprite(LCDSprite* sprite)
{
	if (sprite)
	{
		PlaydateAPI* pd = get_playdate_API();
		BallData* ball_data = (BallData*)pd->sprite->getUserdata(sprite);
		if (ball_data && !ball_is_dead(sprite) && !ball_is_stuck(sprite))
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
						ball_reflect(ball_data, collisions[0].normal);
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

					ball_reflect(ball_data, collisions[0].normal);

					if (collisions[0].normal.y == -1)
                                        {
                                                if (!bend && abs(paddle_get_dx(collisions[0].other)) > 1)
                                                {
                                                        bend = true;
                                                        if (sign(paddle_get_dx(collisions[0].other)) == sign(ball_get_dx(sprite)))
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
                                                                ball_set_angle(sprite, middleOfThree(0, ball_data->angle - 1, 2));
                                                        }
                                                        else
                                                        {
                                                                if (ball_data->angle == 2)
                                                                {
                                                                        ball_data->dx = -ball_data->dx;
                                                                }
                                                                else
                                                                {
                                                                        ball_set_angle(sprite, middleOfThree(0, ball_data->angle + 1, 2));
                                                                }
                                                        }
						}
                                        }
				}

				if (other_tag == BRICK) 
				{
					pd->sprite->removeSprite(collisions[0].other);
					brick_destroy(collisions[0].other);
					ball_reflect(ball_data, collisions[0].normal);
				}
			}
		}
	}
}

void ball_set_dx(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			ball_data->dx = value;
		}
	}
}

float ball_get_dx(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			return ball_data->dx;
		}
	}
}

void ball_set_dy(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			ball_data->dy = value;
		}
	}
}

float ball_get_dy(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			return ball_data->dy;
		}
	}
}

void ball_set_stuck(LCDSprite* sprite, bool stuck)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			ball_data->stuck = stuck;
		}
	}
}

bool ball_is_stuck(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			return ball_data->stuck;
		}
	}
}

bool ball_is_dead(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
		if (ball_data)
		{
			return ball_data->dead;
		}
	}
}

SpriteCollisionResponseType ball_collision_response(LCDSprite* sprite, LCDSprite* other)
{
	return kCollisionTypeBounce;
}

void ball_set_angle(LCDSprite* sprite, float angle)
{
	if (sprite)
        {
                BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
                if (ball_data)
                {
                        ball_data->angle = angle;
                        if (angle == 2.f)
                        {
                                ball_data->dx = .5f * sign(ball_data->dx);
                                ball_data->dy = 1.3f * sign(ball_data->dy);
                        }
                        else if (angle == 0.f)
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

bool ball_rammed(LCDSprite* sprite)
{
	if (sprite)
        {
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
                if (ball_data)
                {
			return ball_data->rammed;
                }
        }
}

void ball_set_rammed(LCDSprite* sprite, bool rammed)
{
	if (sprite)
	{
		BallData* ball_data = (BallData*)get_playdate_API()->sprite->getUserdata(sprite);
                if (ball_data)
                {
			ball_data->rammed = rammed;
		}
	}
}
