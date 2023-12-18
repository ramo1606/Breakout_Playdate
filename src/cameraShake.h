#ifndef CAMERA_SHAKE_H
#define CAMERA_SHAKE_H

#include "pd_api.h"
#include "common.h"

#include <stdbool.h>

void setShake(float value);
float getShake();

void doShake();

#endif