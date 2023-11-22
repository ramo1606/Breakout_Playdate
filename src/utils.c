#include "utils.h"

void UTILS_init()
{
    PlaydateAPI* pd = getPlaydateAPI();

    /* Intializes random number generator */
    srand(pd->system->getSecondsSinceEpoch(NULL));
}

// Function to find the middle of three number
float mid(float a, float b, float c)
{
    // x is positive if a is greater than b.
    // x is negative if b is greater than a.
    float x = a - b;

    float y = b - c;  // Similar to x
    float z = a - c;  // Similar to x and y.

    // Checking if b is middle (x and y both
    // are positive)
    if (x * y > 0)
        return b;

    // Checking if c is middle (x and z both
    // are positive)
    else if (x * z > 0)
        return c;
    else
        return a;
}

int randomInt(int min, int max)
{
    if (min > max)
    {
        int tmp = max;
        max = min;
        min = tmp;
    }

    return (rand() % (abs(max - min) + 1) + min);
}

float randomFloat(float min, float max)
{
    return min + (float)(rand() / ((float)RAND_MAX / (max - min)));
}