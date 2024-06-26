#include "ball.h"

#include "utils.h"
#include "memory.h"
#include "resourcemanager.h"
#include "particles.h"
#include "cameraShake.h"

#include "game.h"
#include "brick.h"
#include "paddle.h"
#include "raymath.h"

#include <stdlib.h>

static const float MAX_COLLISIONS_COUNT = 600.0f;
static const float BALL_DX = 1.f;
static const float BALL_DY = 1.f;
static const float BALL_SPEED = 3.5f;
static const float BALL_ANGLE = 1.f;

static void checkInfinite(LCDSprite* ball) 
{
	BallData* ballData = (BallData*)pd->sprite->getUserdata(ball);
	if (ballData->infiniteCounter > MAX_COLLISIONS_COUNT)
	{
		ballData->infiniteCounter = 0;
		float newAngle = 0.f;
		do 
		{
			newAngle = floorf(randomFloat(0.0f, 3.0f));
		} while (FloatEquals(newAngle, ballData->angle));
		
		BALL_setAngle(ball, newAngle);
	}
}

void BALL_megaBall(LCDSprite* sprite, bool isMega) 
{
	if (sprite)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(sprite);
		if (ballData)
		{
			char* ballName = NULL;
			if (isMega) 
			{
				ballName = "ball_super";
			}
			else 
			{
				ballName = "ball";
			}

			ballData->isMega = isMega;
			pd->sprite->setImage(sprite, RESOURCEMANAGER_getImage(ballName), kBitmapUnflipped);

			int w, h;
			pd->graphics->getBitmapData(RESOURCEMANAGER_getImage(ballName), &w, &h, NULL, NULL, NULL);

			float x, y;
			pd->sprite->getPosition(sprite, &x, &y);

			// Create bounds rect for paddle
			PDRect bounds = PDRectMake(0.f, 0.f, (float)w, (float)h);
			pd->sprite->setBounds(sprite, bounds);

			// Create collision rect for paddle
			PDRect cr = PDRectMake(1.f, 1.f, (float)(w - 2), (float)(h - 2));
			pd->sprite->setCollideRect(sprite, cr);

			pd->sprite->moveTo(sprite, x, y);
		}
	}
}

LCDSprite* BALL_create(float x, float y)
{
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
	ballData->lastHitX = 0.f;
	ballData->lastHitY = 0.f;
	ballData->speed = BALL_SPEED;
	ballData->angle = BALL_ANGLE;
	ballData->isStuck = false;
	ballData->isDead = false;
	ballData->rammed = false;
	ballData->isMega = false;
	ballData->collisionCount = 0;
	ballData->infiniteCounter = 0.0f;
	ballData->timerSlow = 0;
	ballData->timerMegaWait = 0;
	ballData->timerMega = 0;
	ballData->lastCollision = NONE;
	pd->sprite->setUserdata(ball, (void*)ballData);
	return ball;
}

void BALL_destroy(LCDSprite* sprite)
{
	pd->sprite->removeSprite(sprite);
	BallData* ballData = (BallData*)pd->sprite->getUserdata(sprite);
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
		BallData* ballData = (BallData*)pd->sprite->getUserdata(sprite);
		PDRect ball_bounds = pd->sprite->getBounds(sprite);

		if (ballData->timerSlow > 0) 
		{
			ballData->infiniteCounter += 0.5f;
		}
		else 
		{
			ballData->infiniteCounter += 1.0f;
		}

		if ((ballData->timerMegaWait > 0 || ballData->timerMega > 0) && !ballData->isMega) 
		{
			BALL_megaBall(sprite, true);
		}
		else if (ballData->timerMegaWait <= 0 && ballData->timerMega <= 0 && ballData->isMega)
		{
			BALL_megaBall(sprite, false);
		}

		if(ballData->isStuck)
		{
			// Get all the data that we need to calculate the ball position if is stuck, which is on the top of the paddle
			LCDSprite* paddle = GAME_getPaddle();
			float pad_x = 0;
			float pad_y = 0;
			pd->sprite->getPosition(paddle, &pad_x, &pad_y);
			PDRect pad_bounds = pd->sprite->getBounds(paddle);
			pd->sprite->moveTo(sprite, pad_x, pad_y - (pad_bounds.height * 0.5f) - (ball_bounds.height * 0.5f));
			ballData->infiniteCounter = 0.f;
		}
		else 
		{
			float x = 0.0f;
			float y = 0.0f;
			pd->sprite->getPosition(sprite, &x, &y);

			float speedModifier = 1.0f;
			if (ballData->timerSlow > 0)
			{
				speedModifier = 0.5f;
			}

			float nextx = x + (ballData->dx * speedModifier * ballData->speed);
			float nexty = y + (ballData->dy * speedModifier * ballData->speed);

			PDRect ball_rect = pd->sprite->getCollideRect(sprite);

			// Move the ball and check for collisions
			int len = 0;
			float actual_x = 0;
			float actual_y = 0;
			SpriteCollisionInfo* collisions = NULL;
			collisions = pd->sprite->moveWithCollisions(sprite, nextx, nexty, &actual_x, &actual_y, &len);

			//Save speed before collision
			ballData->lastHitX = ballData->dx;
			ballData->lastHitY = ballData->dy;

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
				
				if (!ballData->isMega)
				{
					BALL_processCollision(sprite, &collisions[nearestCollision], actual_x, actual_y);
				}
				else 
				{
					BALL_processMegaCollision(sprite, &collisions[nearestCollision], actual_x, actual_y);
				}
			}

			// Process mega ball collision
			

			// Trail particles
			if (ballData->timerMega > 0 || ballData->timerMegaWait > 0) 
			{
				BALL_spawnMegaTrail(x, y, ball_bounds.width * 0.5f);
			}
			else 
			{
				BALL_spawnTrail(x, y, ball_bounds.width * 0.5f);
			}
			// TODO: Process screen boundaries collision
			// TODO: process ball is stuck
		}

		// Powerup timers
		if (ballData->timerSlow > 0)
		{
			ballData->timerSlow--;
		}
		if (ballData->timerMega > 0)
		{
			ballData->timerMega--;
		}
		if (ballData->timerMegaWait > 0)
		{
			ballData->timerMegaWait--;
		}
	}
}

void BALL_deactivate(LCDSprite* sprite)
{
	pd->sprite->setUpdatesEnabled(sprite, 0);
	pd->sprite->setVisible(sprite, 0);
	pd->sprite->setCollisionsEnabled(sprite, 0);

	BallData* ballData = (BallData*)pd->sprite->getUserdata(sprite);
	ballData->isDead = true;
}

void BALL_activate(LCDSprite* sprite)
{
	pd->sprite->setUpdatesEnabled(sprite, 1);
	pd->sprite->setVisible(sprite, 1);
	pd->sprite->setCollisionsEnabled(sprite, 1);
	
	BallData* ballData = (BallData*)pd->sprite->getUserdata(sprite);
	ballData->isDead = false;
}

SpriteCollisionResponseType BALL_collisionResponse(LCDSprite* sprite, LCDSprite* other)
{
	if (pd->sprite->getTag(other) == PILL) 
	{
		return kCollisionTypeOverlap;
	}
	else 
	{
		return kCollisionTypeBounce;
	}
}

void BALL_setAngle(LCDSprite* sprite, float angle)
{
	if (sprite)
    {
		BallData* ballData = (BallData*)pd->sprite->getUserdata(sprite);
		if (ballData)
		{
			ballData->angle = angle;
			if (FloatEquals(angle, 2.f))
			{
				ballData->dx = .5f * sign(ballData->dx);
				ballData->dy = 1.3f * sign(ballData->dy);
			}
			else if (FloatEquals(angle, 0.f))
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

void BALL_resetBall(LCDSprite* sprite)
{
	BallData* ballData = (BallData*)pd->sprite->getUserdata(sprite);

	ballData->dx = BALL_DX;
	ballData->dy = BALL_DY;
	ballData->lastHitX = 0.f;
	ballData->lastHitY = 0.f;
	ballData->speed = BALL_SPEED;
	ballData->angle = BALL_ANGLE;
	ballData->isStuck = false;
	ballData->isDead = false;
	ballData->rammed = false;
	ballData->isMega = false;
	ballData->collisionCount = 0;
	ballData->infiniteCounter = 0.0f;
	ballData->timerSlow = 0;
	ballData->timerMegaWait = 0;
	ballData->timerMega = 0;
	ballData->lastCollision = NONE;
}

void BALL_copyBall(LCDSprite* original, LCDSprite* copy)
{
	BallData* ballData = (BallData*)pd->sprite->getUserdata(original);
	BallData* copyData = (BallData*)pd->sprite->getUserdata(copy);

	copyData->dx = ballData->dx;
	copyData->dy = ballData->dy;
	copyData->lastHitX = ballData->lastHitX;
	copyData->lastHitY = ballData->lastHitY;
	copyData->speed = ballData->speed;
	copyData->angle = ballData->angle;
	copyData->isStuck = ballData->isStuck;
	copyData->isDead = ballData->isDead;
	copyData->rammed = ballData->rammed;
	copyData->isMega = ballData->isMega;
	copyData->collisionCount = ballData->collisionCount;
	copyData->infiniteCounter = ballData->infiniteCounter;
	copyData->timerSlow = ballData->timerSlow;
	copyData->timerMegaWait = ballData->timerMegaWait;
	copyData->timerMega = ballData->timerMega;
	copyData->lastCollision = ballData->lastCollision;
}

void BALL_megaballSmash(LCDSprite* sprite)
{
	if (sprite)
	{
		BallData* ballData = (BallData*)pd->sprite->getUserdata(sprite);
		if (ballData)
		{
			if (ballData->timerMegaWait > 0) 
			{
				ballData->timerMegaWait = 0;
				ballData->timerMega = 120;
			}
		}
	}
}

void BALL_processCollision(LCDSprite* sprite, SpriteCollisionInfo* collision, float x, float y)
{
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
			ballData->isDead = true;
			BALL_deactivate(sprite);
			BALL_spawnDeath(collision->touch.x, collision->touch.y);
			SHAKE_set(SHAKE_get() + 0.4f);
		}
		else
		{
			BALL_reflect(ballData, collision->normal);
			checkInfinite(sprite);
			
			BALL_spawnPuft(x, y);
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
		PaddleData* paddleData = (PaddleData*)pd->sprite->getUserdata(collision->other);

		bool bend = false;
		bool angf = false;
		ballData->infiniteCounter = 0;
		GAME_resetChain();
		
		GAME_checkSuddenDeath();

		if (collision->normal.x != 0)
		{
			if (y + (ballRect.height * 0.5f) > pad_y + (ballRect.height * 0.5f) + 2.f)
			{
				ballData->rammed = true;
				BALL_spawnPuft(x, y);
			}
			else
			{
				bend = true;
				angf = false;
				collision->normal.y = -1;
				pd->sprite->moveTo(sprite, x, pad_y - (ballRect.height * 0.5f));
			}
		}

		BALL_reflect(ballData, collision->normal);

		// Change angle
		if (collision->normal.y == -1)
		{
			if (!bend && fabs(paddleData->dx) > 1.0f)
			{
				bend = true;
				if (sign(paddleData->dx) == sign(ballData->dx))
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
					if (FloatEquals(ballData->angle, 2.f))
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

			// Catch Powerup
			if (paddleData->sticky)
			{
				GAME_releaseStuck();
				paddleData->sticky = false;
				ballData->isStuck = true;
			}
		}

		BALL_spawnPuft(x, y);
		//TODO:  puft and sound
	}

	// BRICK Collision
	if (other_tag == BRICK)
	{
		BrickData* brickData = (BrickData*)pd->sprite->getUserdata(collision->other);
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
		GAME_hitBrick(collision, true);

		if (brickData->type == INVINCIBLE)
		{
			BALL_spawnPuft(x, y);
		}
	}
}

void BALL_processMegaCollision(LCDSprite* sprite, SpriteCollisionInfo* collision, float x, float y) 
{
}

void BALL_spawnMegaTrail(float x, float y, float radius)
{
	if (randomFloat(0.0f, 1.0f) < 0.7f)
	{
		float ang = randomFloat(0.0f, PI * 2.f);
		float ox = sinf(ang) * radius * 0.15f;
		float oy = cosf(ang) * radius * 0.15f;
		LCDColor colors[3] = { ditheringPatterns[2], ditheringPatterns[4], ditheringPatterns[8] };
		PARTICLES_addParticle(x + ox, y + oy, 0.0f, 0.0f, SMOKE_BALL, 60.f + randomFloat(0.0f, 15.0f), colors, 3, 8.f + randomFloat(0.0f, 2.0f));
	}
}

void BALL_spawnTrail(float x, float y, float radius)
{
	if (randomFloat(0.0f, 1.0f) < 0.8f)
	{
		float ang = randomFloat(0.0f, PI * 2.f);
		float ox = sinf(ang) * radius * 0.15f;
		float oy = cosf(ang) * radius * 0.15f;
		LCDColor colors[1] = { kColorBlack };
		PARTICLES_addParticle(x + ox, y + oy, 0.0f, 0.0f, STATIC_PIX, 15 + randomFloat(0.0f, 15.0f), colors, 1, 0.0f);
	}
}

void BALL_spawnPuft(float x, float y)
{
	for (size_t i = 0; i < 5; i++)
	{
		float ang = randomFloat(0.0f, PI * 2.f);
		float dx = sinf(ang) * 3.0f;
		float dy = cosf(ang) * 3.0f;
		LCDColor colors[3] = { ditheringPatterns[2], ditheringPatterns[4], ditheringPatterns[8] };
		PARTICLES_addParticle(x, y, dx, dy, SMOKE_BALL, 15.0f + randomFloat(0.0f, 15.0f), colors, 3, 4 + randomFloat(0.0f, 6.0f));
	}
}

void BALL_spawnDeath(float x, float y)
{
	for (size_t i = 0; i < 30; ++i)
	{
		float ang = randomFloat(0.0f, PI * 2.f);
		float dx = sinf(ang) * (4.f + randomFloat(0.f, 6.f));
		float dy = cosf(ang) * (4.f + randomFloat(0.f, 6.f));
		LCDColor colors[6] = { ditheringPatterns[2], ditheringPatterns[4], ditheringPatterns[4], ditheringPatterns[8], ditheringPatterns[10], ditheringPatterns[10] };
		PARTICLES_addParticle(x, y, dx, dy, SMOKE_BALL, 80 + randomFloat(0.0f, 15.0f), colors, 6, 6 + randomFloat(0.0f, 10.0f));
	}
}