#pragma once
#include "pd_api.h"

typedef struct Resources Resources;

Resources* load_resources();
LCDBitmap* get_image(Resources* resources, const char* image);