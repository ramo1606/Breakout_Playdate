#pragma once
#include "pd_api.h"
#include <stdbool.h>
#include <math.h>

#define EPSILON 0.0001
#define sign(x)  (signbit(x) ?  -1 : 1)

bool AreSame(float a, float b);

// Function to find the middle of three number
float middleOfThree(float a, float b, float c);
