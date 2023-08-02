#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "pd_api.h"

/* Load resources */
void RESOURCEMANAGER_load(void);

/* Unload resources */
void RESOURCEMANAGER_unLoad(void);

/* Get image resources by name */
LCDBitmap* RESOURCEMANAGER_getImage(const char* image);

#endif
