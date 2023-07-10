#pragma once
#include "pd_api.h"
#include <stdbool.h>

#define EPSILON 0.0001

inline bool AreSame(float a, float b)
{
    return fabs(a - b) < EPSILON;
}