//
//  main.c
//

#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

#include "engine.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if (event == kEventInit)
	{
		ENGINE_create(pd);
		pd->display->setRefreshRate(30);
		pd->system->setUpdateCallback(ENGINE_update, NULL);
	}

	return 0;
}
