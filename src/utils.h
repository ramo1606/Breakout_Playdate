#ifndef UTILS_H
#define UTILS_H

#include "pd_api.h"
#include "common.h"

#include <stdbool.h>
#include <math.h>

#define EPSILON 0.0001
#define sign(x)  (signbit(x) ?  -1 : 1)

/* Compares two float numbers */
bool areEqual(float a, float b);

/* Function to find the middle of three number */
float mid(float a, float b, float c);

#endif
