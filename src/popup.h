#ifndef POPUP_H
#define POPUP_H

#include "common.h"

#define STRMAX 128

typedef struct PopupData
{
	float dx;
	float dy;
	float destX;
	float destY;
	float originalX;
	float originalY;
	float animationElapsedTime;
	float animationTime;
	char* message[STRMAX];
	size_t messageLen;
} PopupData;

LCDSprite* POPUP_create(char* message, size_t messageLen);
void POPUP_destroy(LCDSprite* sprite);
void POPUP_updateSprite(LCDSprite* sprite);

#endif