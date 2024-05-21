#include "sash.h"

static float sashWidth = 0;
static float sashDW = 0;
static float sashTextX = 0;
static float sashTextDX = 0;
static LCDColor sashColor = kColorClear;
static LCDColor sashTextColor = kColorClear;
static char* sashText = "ohai";
static int sashFrames = 0;
static bool sashVisible = false;
static int sashDelayText = 0;
static int sashDelayWidth = 0;

void SASH_update(void)
{
	if (sashVisible) 
	{
		sashFrames += 1;
		if (sashDelayWidth > 0) 
		{
			sashDelayWidth -= 1;
		}
		else 
		{
			sashWidth += (sashDW - sashWidth) / 5;
			if (fabs(sashDW - sashWidth) < 0.3f) 
			{
				sashWidth = sashDW;
			}
		}

		// Animate Text
		if (sashDelayText > 0) 
		{
			sashDelayText -= 1;
		}
		else 
		{
			sashTextX += (sashTextDX - sashTextX) * 0.1f;
			if (fabs(sashTextX - sashTextDX) < 0.3f)
			{
				sashTextX = sashTextDX;
			}
		}

		// Make sash go away
		if (sashFrames == 75) 
		{
			sashDW = 0.f;
			sashTextDX = 400.f + pd->graphics->getTextWidth(font, sashText, (int)strlen(sashText), kASCIIEncoding, 0);
			sashDelayWidth = 15;
			sashDelayText = 0;
		}
		if (sashFrames > 115) 
		{
			sashVisible = false;
		}
	}
}

void SASH_draw(void)
{
	LCDColor color;
	int i;

	if (sashVisible) 
	{
		color = sashColor;
		//if (sashColor == kColorClear) 
		//{
		//	color = sashColor == kColorBlack ? kColorWhite : kColorBlack;
		//}
		//else 
		//{
		//	color = sashColor;
		//}

		pd->graphics->fillRect(0, 120 - sashWidth, 400, sashWidth * 2, color);
		pd->graphics->setDrawMode(kDrawModeFillWhite);
		int16_t fontHeihgt = pd->graphics->getFontHeight(font);
		pd->graphics->drawText(sashText, strlen(sashText), kASCIIEncoding, sashTextX, 120 - (fontHeihgt * 0.5f) + 2);
		pd->graphics->setDrawMode(kDrawModeCopy);
		pd->graphics->setClipRect(0, 120 - sashWidth, 400, sashWidth * 2 + 1);
		pd->graphics->clearClipRect();
	}
}

void SASH_show(char* text, LCDColor color, LCDColor textColor)
{
	strcpy(sashText, text);
	sashWidth = 0;
	sashDW = 15;
	sashColor = color;
	sashFrames = 0;
	sashVisible = true;
	sashTextX = - pd->graphics->getTextWidth(font, sashText, (int)strlen(sashText), kASCIIEncoding, 0);
	sashTextDX = 200 - pd->graphics->getTextWidth(font, sashText, (int)strlen(sashText), kASCIIEncoding, 0) / 2;
	sashDelayWidth = 0;
	sashDelayText = 16;
	sashTextColor = textColor;
}
