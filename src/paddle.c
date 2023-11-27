#include "paddle.h"

#include "memory.h"
#include "particles.h"
#include "resourcemanager.h"
#include "utils.h"

#include "ball.h"

void PADDLE_spawnSpeedLines(LCDSprite* sprite, float x, float y)
{
	PlaydateAPI* pd = getPlaydateAPI();
	PaddleData* paddle_data = (PaddleData*)pd->sprite->getUserdata(sprite);

	if (randomFloat(0.0f, 1.0f) < 0.2f)
	{
		PDRect pad_bounds = pd->sprite->getBounds(sprite);
		float ox = randomFloat(0.0f, 1.0f) * 2.5f;
		float oy = randomFloat(0.0f, 1.0f) * pad_bounds.height;

		PARTICLES_addParticle(x + ox, (y - pad_bounds.height * 0.5f) + oy, paddle_data->dx, 0.f, SPEED_LINE, 10.f + randomFloat(0.0f, 15.0f), kColorBlack, 6.f + randomFloat(0.0f, 10.0f));
	}
}

void PADDLE_changeSize(LCDSprite* sprite, EPaddleSize newSize)
{
	if (sprite)
	{
		PlaydateAPI* pd = getPlaydateAPI();
		PaddleData* paddleData = (PaddleData*)pd->sprite->getUserdata(sprite);
		if (paddleData)
		{
			char* paddleName = NULL;
			switch (newSize)
			{
			case SHORT:
				paddleName = "paddle_s";
				break;
			case MEDIUM:
				paddleName = "paddle_m";
				break;
			case LARGE:
				paddleName = "paddle_l";
				break;
			default:
				break;
			}

			paddleData->currentSize = newSize;
			pd->sprite->setImage(sprite, RESOURCEMANAGER_getImage(paddleName), kBitmapUnflipped);

			int w, h;
			pd->graphics->getBitmapData(RESOURCEMANAGER_getImage(paddleName), &w, &h, NULL, NULL, NULL);

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

void PADDLE_reset(LCDSprite* sprite)
{
}

LCDSprite* PADDLE_create(float x, float y)
{
	PlaydateAPI* pd = getPlaydateAPI();

	// Create new sprite
	LCDSprite* paddle = pd->sprite->newSprite();

	pd->sprite->setUpdateFunction(paddle, PADDLE_updateSprite);
	pd->sprite->setImage(paddle, RESOURCEMANAGER_getImage("paddle_m"), kBitmapUnflipped);

	int w, h;
	pd->graphics->getBitmapData(RESOURCEMANAGER_getImage("paddle_m"), &w, &h, NULL, NULL, NULL);

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
	PaddleData* paddleData = pd_malloc(sizeof(PaddleData));
	paddleData->dx = 0.f;
	paddleData->speedWind = 0;
	paddleData->timerExpand = 0;
	paddleData->timerReduce = 0;
	paddleData->currentSize = MEDIUM;
	pd->sprite->setUserdata(paddle, (void*)paddleData);

	return paddle;
}

void PADDLE_destroy(LCDSprite* sprite)
{
	getPlaydateAPI()->sprite->removeSprite(sprite);
	PaddleData* paddleData = (PaddleData*)getPlaydateAPI()->sprite->getUserdata(sprite);
	pd_free(paddleData);
	pd_free(sprite);
}

void PADDLE_updateSprite(LCDSprite* sprite)
{
	if (sprite)
	{
		PlaydateAPI* pd = getPlaydateAPI();
		PaddleData* paddleData = (PaddleData*)pd->sprite->getUserdata(sprite);
		if (paddleData)
		{
			if (paddleData->timerExpand > 0 && paddleData->currentSize != LARGE)
			{
				PADDLE_changeSize(sprite, LARGE);
			}
			else if (paddleData->timerReduce > 0 && paddleData->currentSize != SHORT)
			{
				PADDLE_changeSize(sprite, SHORT);
			}
			else 
			{
				if (paddleData->timerExpand <= 0 && paddleData->timerReduce <= 0 && paddleData->currentSize != MEDIUM)
				{
					PADDLE_changeSize(sprite, MEDIUM);
				}
			}
			
			float pad_x = 0;
			float pad_y = 0;
			pd->sprite->getPosition(sprite, &pad_x, &pad_y);

			PDRect pad_rect = pd->sprite->getCollideRect(sprite);

			float actual_x = 0;
			float actual_y = 0;
			int len = 0;
			float oldPad_x = pad_x;
			pd->sprite->moveTo(sprite, mid((pad_rect.width * 0.5f), pad_x + paddleData->dx, (pd->display->getWidth() - (pad_rect.width * 0.5f))), pad_y);

			pd->sprite->getPosition(sprite, &pad_x, &pad_y);

			if(paddleData->speedWind > 5)
			{
				if(paddleData->dx < 0.0f)
				{
					PADDLE_spawnSpeedLines(sprite, pad_x + (pad_rect.width * 0.5f), pad_y);
				}
				else
				{
					PADDLE_spawnSpeedLines(sprite, pad_x - (pad_rect.width * 0.5f), pad_y);
				}
			}

			// Powerup timers
			if (paddleData->timerExpand > 0)
			{
				paddleData->timerExpand--;
			}
			if (paddleData->timerReduce > 0)
			{
				paddleData->timerReduce--;
			}
		}
	}
}

SpriteCollisionResponseType PADDLE_collisionResponse(LCDSprite* sprite, LCDSprite* other)
{
	return kCollisionTypeOverlap;
}