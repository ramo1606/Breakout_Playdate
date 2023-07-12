#include "common.h"

// Playdate API
static PlaydateAPI* pd = NULL;

void set_playdate_API(PlaydateAPI* playdate)
{
	pd = playdate;
}

PlaydateAPI* get_playdate_API()
{
	return pd;
}