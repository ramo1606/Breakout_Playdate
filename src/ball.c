#include "ball.h"

#include "common.h"
#include "utils.h"
#include "memory.h"
#include "resourcemanager.h"
#include "particles.h"

#include "gamestate.h"
#include "brick.h"
#include "paddle.h"

#include <stdlib.h>
#include <time.h>

static const int MAX_COLLISIONS_COUNT = 600;
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
	int infiniteCounter;
	ESpriteType lastCollision;
};

void checkInfinite(LCDSprite* ball) 
{
	BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(ball);
	if (ballData->infiniteCounter > MAX_COLLISIONS_COUNT)
	{
		ballData->infiniteCounter = 0;
		float newAngle = 0.f;
		do 
		{
			newAngle = floorf(randomFloat(0.0f, 3.0f));
		} while (areEqual(newAngle, ballData->angle));
		
		BALL_setAngle(ball, newAngle);
	}
}

LCDSprite* BALL_create(float x, float y)
{
	PlaydateAPI* pd = getPlaydateAPI();

	LCDSprite* ball = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(ball, BALL_updateSprite);
	pd->sprite->setImage(ball, RESOURCEMANAGER_getImage("ball"), kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(RESOURCEMANAGER_getImage("ball"), &w, &h, NULL, NULL, NULL);

	// Create bounds rect for ball
	PDRect bounds = PDRectMake(0.f, 0.f, (float)w, (float)h);
	pd->sprite->setBounds(ball, bounds);

	// Create collision rect for ball and set collisions to bounce
	PDRect cr = PDRectMake(1.f, 1.f, (float)(w - 2), (float)(h - 2));
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
	ballData->infiniteCounter = 0;
	ballData->lastCollision = NONE;
	pd->sprite->setUserdata(ball, (void*)ballData);
	return ball;
}

void BALL_destroy(LCDSprite* sprite)
{
	getPlaydateAPI()->sprite->removeSprite(sprite);
	BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
	pd_free(ballData);
	pd_free(sprite);
}

void BALL_reflect(BallData* ballData, CollisionVector normal)
{
	if (normal.x != 0)
	{
		ballData->dx = -ballData->dx;
	}
	if (normal.y != 0)
	{
		ballData->dy = -ballData->dy;
	}
}

void BALL_updateSprite(LCDSprite* sprite)
{
	if (sprite)
	{
		PlaydateAPI* pd = getPlaydateAPI();
		BallData* ballData = (BallData*)pd->sprite->getUserdata(sprite);
		PDRect ball_bounds = pd->sprite->getBounds(sprite);

		if(BALL_isStuck(sprite))
		{
			// Get all the data that we need to calculate the ball position if is stuck, which is on the top of the paddle
			LCDSprite* paddle = GAMESTATE_getPaddle();
			float pad_x = 0;
			float pad_y = 0;
			pd->sprite->getPosition(paddle, &pad_x, &pad_y);
			PDRect pad_bounds = pd->sprite->getBounds(paddle);
			pd->sprite->moveTo(sprite, pad_x, pad_y - (pad_bounds.height * 0.5f) - (ball_bounds.height * 0.5));
			ballData->infiniteCounter = 0.f;
		}
		else 
		{
			float x = 0;
			float y = 0;
			pd->sprite->getPosition(sprite, &x, &y);

			//TODO: add time slow case
			float nextx = x + ballData->dx * ballData->speed;
			float nexty = y + ballData->dy * ballData->speed;

			PDRect ball_rect = pd->sprite->getCollideRect(sprite);

			// Move the ball and check for collisions
			int len = 0;
			float actual_x = 0;
			float actual_y = 0;
			SpriteCollisionInfo* collisions = NULL;
			collisions = pd->sprite->moveWithCollisions(sprite, nextx, nexty, &actual_x, &actual_y, &len);

			// check if there is any collision
			if (len > 0	)
			{
				//Just check for the nearest collision to the ball
				int nearestCollision = 0;
				float collisionDist = collisions[0].ti;
				for (int i = 1; i < len; i++)
				{
					if (collisionDist > collisions[i].ti) 
					{
						collisionDist = collisions[i].ti;
						nearestCollision = i;
					}
				}

				BALL_processCollision(sprite, &collisions[nearestCollision], actual_x, actual_y);
			}

			// TODO: Process mega ball collision
			// TODO: Trail particles
			PARTICLES_spawnTrail(x, y, ball_bounds.width * 0.5f);
			// TODO: Process screen boundaries collision
			// TODO: process ball is stuck
		}
	}
}

void BALL_setDx(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ballData)
		{
			ballData->dx = value;
		}
	}
}

float BALL_getDx(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ballData)
		{
			return ballData->dx;
		}
	}
	return 0;
}

void BALL_setDy(LCDSprite* sprite, float value)
{
	if (sprite)
	{
		BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ballData)
		{
			ballData->dy = value;
		}
	}
}

float BALL_getDy(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ballData)
		{
			return ballData->dy;
		}
	}
	return 0;
}

void BALL_setStuck(LCDSprite* sprite, bool stuck)
{
	if (sprite)
	{
		BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ballData)
		{
			ballData->stuck = stuck;
		}
	}
}

bool BALL_isStuck(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ballData)
		{
			return ballData->stuck;
		}
	}
	return false;
}

bool BALL_isDead(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ballData)
		{
			return ballData->dead;
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
		BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ballData)
		{
			ballData->angle = angle;
			if (areEqual(angle, 2.f))
			{
				ballData->dx = .5f * sign(ballData->dx);
				ballData->dy = 1.3f * sign(ballData->dy);
			}
			else if (areEqual(angle, 0.f))
			{
				ballData->dx = 1.3f * sign(ballData->dx);
				ballData->dy = .5f * sign(ballData->dy);
			}
			else
			{
				ballData->dx = 1.f * sign(ballData->dx);
				ballData->dy = 1.f * sign(ballData->dy);
			}
		}
    }
}

bool BALL_rammed(LCDSprite* sprite)
{
	if (sprite)
    {
		BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
		if (ballData)
		{
			return ballData->rammed;
        }
    }
	return false;
}

void BALL_setRammed(LCDSprite* sprite, bool rammed)
{
	if (sprite)
	{
		BallData* ballData = (BallData*)getPlaydateAPI()->sprite->getUserdata(sprite);
        if (ballData)
        {
			ballData->rammed = rammed;
		}
	}
}

void BALL_processCollision(LCDSprite* sprite, SpriteCollisionInfo* collision, float x, float y)
{
	PlaydateAPI* pd = getPlaydateAPI();
	BallData* ballData = (BallData*)pd->sprite->getUserdata(sprite);

	// Move the ball to collision position and add collision to collisionCount
	pd->sprite->moveTo(sprite, x, y);
	ballData->collisionCount++;
	
	uint8_t other_tag = pd->sprite->getTag(collision->other);

	// WALL collision 
	if (other_tag == WALL)
	{
		if (collision->normal.y == -1)
		{
			ballData->dead = true;
		}
		else
		{
			BALL_reflect(ballData, collision->normal);
			checkInfinite(sprite);
			
			PARTICLES_spawnPuft(x, y);
			//TODO: sound effect
		}
	}

	// PADDLE Collision
	if (other_tag == PADDLE)
	{
		float pad_x = 0;
		float pad_y = 0;
		pd->sprite->getPosition(collision->other, &pad_x, &pad_y);
		PDRect padRect = pd->sprite->getCollideRect(collision->other);
		PDRect ballRect = pd->sprite->getCollideRect(sprite);

		bool bend = false;
		bool angf = false;
		ballData->infiniteCounter = 0;
		//TODO: reset chain
		//TODO: check suden death
		if (collision->normal.x != 0)
		{
			// TODO: check pixel perfect when having the new assets
			if (y + (ballRect.height * 0.5f) > pad_y - (padRect.height * 0.5f) + 3)
			{
				ballData->rammed = true;
				PARTICLES_spawnPuft(x, y);
			}
			else
			{
				bend = true;
				angf = false;
				collision->normal.y = -1;
				pd->sprite->moveTo(sprite, x, pad_y - (padRect.height * 0.5f) - (ballRect.height * 0.5));
			}
		}

		BALL_reflect(ballData, collision->normal);

		// Change angle
		if (collision->normal.y == -1)
		{
			if (!bend && abs(PADDLE_getDx(collision->other)) > 1)
			{
				bend = true;
				if (sign(PADDLE_getDx(collision->other)) == sign(BALL_getDx(sprite)))
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
					// Flatten angle
					BALL_setAngle(sprite, mid(0.f, ballData->angle - 1.f, 2.f));
				}
				else
				{
					// Raise angle
					if (areEqual(ballData->angle, 2.f))
					{
						ballData->dx = -ballData->dx;
					}
					else
					{
						BALL_setAngle(sprite, mid(0.f, ballData->angle + 1.f, 2.f));
					}
				}
			}
			// Reset Position
			PDRect pad_bounds = pd->sprite->getBounds(collision->other);
			PDRect ball_bounds = pd->sprite->getBounds(sprite);
			pd->sprite->moveTo(sprite, x, pad_y - (pad_bounds.height * 0.5f) - (ball_bounds.height * 0.5));

			//TODO: Catch Powerup
			//if (sticky) 
			//{
			//	if sticky then
			//		releasestuck()
			//		sticky = false
			//		_b.stuck = true
			//		sticky_x = _b.x - pad_x
			//	end
			//}
		}

		PARTICLES_spawnPuft(x, y);
		//TODO:  puft and sound
	}

	// BRICK Collision
	if (other_tag == BRICK)
	{
		if (ballData->lastCollision == NONE)
		{
			ballData->lastCollision = BRICK;
		}
		else 
		{
			if (ballData->lastCollision != BRICK) 
			{
				ballData->collisionCount = 0;
			}
		}
		BALL_reflect(ballData, collision->normal);
		checkInfinite(sprite);

		// TODO: HitBrick
		if (BRICK_getType(collision->other) == INVINCIBLE)
		{
			PARTICLES_spawnPuft(x, y);
		}
		pd->sprite->removeSprite(collision->other);
		BRICK_destroy(collision->other);
	}
}