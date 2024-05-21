#include "popup.h"

#include "memory.h"
#include "raymath.h"
#include "easing.h"
#include "rmem.h"
#include "game.h"

static void animatePopup(LCDSprite* sprite)
{
	PopupData* popupData = (PopupData*)pd->sprite->getUserdata(sprite);
	if (popupData->animationElapsedTime <= popupData->animationTime)
	{
		//float x, y = 0.0f;
		//pd->sprite->getPosition(sprite, &x, &y);
		float newX = Lerp(popupData->originalX, popupData->destX, easeOutElastic(popupData->animationElapsedTime / popupData->animationTime));

		pd->sprite->moveTo(sprite, newX, popupData->originalY);
		float deltaTime = GAME_deltaTime();
		popupData->animationElapsedTime = popupData->animationElapsedTime + deltaTime;
	}
}

LCDSprite* POPUP_create(char* message, size_t messageLen) 
{
	// Create new Sprite
	LCDSprite* popup = pd->sprite->newSprite();

	// Set update function
	pd->sprite->setUpdateFunction(popup, POPUP_updateSprite);

	// Set bitmap
	pd->sprite->setImage(popup, pd->graphics->newBitmap(200, 120, kColorWhite), kBitmapUnflipped);

	// Get width and height of the bitmap
	int w, h;
	pd->graphics->getBitmapData(pd->sprite->getImage(popup), &w, &h, NULL, NULL, NULL);

	// Create bounds rect for popup
	PDRect bounds = PDRectMake(0.f, 0.f, (float)w, (float)h);
	pd->sprite->setBounds(popup, bounds);

	// Calculate position
	float x = 401 + (w/2.f);
	float y = 120;
	pd->sprite->moveTo(popup, x, y);
	pd->sprite->setZIndex(popup, 10);
	pd->sprite->setTag(popup, UI);
	pd->sprite->addSprite(popup);

	// Initialize paddle data
	PopupData* popupData = pd_malloc(sizeof(PopupData));
	popupData->dx = -20.f;
	popupData->dy = 0.f;
	popupData->destX = 200;
	popupData->destY = y;
	popupData->originalX = x;
	popupData->originalY = y;
	popupData->animationElapsedTime = 0.0f;
	popupData->animationTime = 1.2f;
	popupData->messageLen = strncpy(popupData->message, message, STRMAX);
	pd->sprite->setUserdata(popup, (void*)popupData);

	pd->graphics->pushContext(pd->sprite->getImage(popup));
	pd->graphics->drawRect(0, 0, w, h, kColorBlack);
	int textWidth1 = pd->graphics->getTextWidth(font, message, 12, kASCIIEncoding, pd->graphics->getTextTracking());
	pd->graphics->drawText(message, 12, kASCIIEncoding, (w/2) - (textWidth1 * 0.5f), 30);
	//int textWidth2 = pd->graphics->getTextWidth(font, "PRESS A or -> TO CONTINUE", 12, kASCIIEncoding, pd->graphics->getTextTracking());
	//pd->graphics->drawText("PRESS A or -> TO CONTINUE", 12, kASCIIEncoding, (w/2) - (textWidth2 * 0.5f), 60);
	pd->graphics->popContext();

	return popup;
}

void POPUP_destroy(LCDSprite* sprite) 
{
	pd->sprite->removeSprite(sprite);
	PopupData* popupData = (PopupData*)pd->sprite->getUserdata(sprite);
	pd_free(popupData);
	pd_free(sprite);
}

void POPUP_updateSprite(LCDSprite* sprite) 
{
	if (sprite)
	{
		animatePopup(sprite);
	}
}