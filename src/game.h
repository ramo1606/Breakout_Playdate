#pragma once
#include "pd_api.h"
#include <stdbool.h>

#define EPSILON 0.0001

void create_game(PlaydateAPI* p);

int update();