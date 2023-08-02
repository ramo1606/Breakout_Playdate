#include "common.h"

// Playdate API
static PlaydateAPI* pd = NULL;

void setPlaydateAPI(PlaydateAPI* playdate)
{
	pd = playdate;
}

PlaydateAPI* getPlaydateAPI(void)
{
	return pd;
}
