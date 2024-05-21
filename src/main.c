//
//  main.c
//

#include "game.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if (event == kEventInit)
	{
		GAME_create(pd);
		pd->display->setRefreshRate(30);
		pd->system->setUpdateCallback(GAME_update, NULL);
	}

	if (event == kEventTerminate) 
	{
		GAME_destroy();
	}

	return 0;
}
