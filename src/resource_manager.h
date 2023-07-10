#pragma once
#include "pd_api.h"

typedef struct Resources Resources;

Resources* load_resources(PlaydateAPI* playdate);
LCDBitmap* get_image(Resources* resources, const char* image);