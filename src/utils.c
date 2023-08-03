#include "utils.h"

static float time = 0;

bool areEqual(float a, float b)
{
    return fabs(a - b) < EPSILON;
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

void transitionUpdate(void)
{
    time += 0.15f;
}

void transitionDraw(void)
{
    // TODO: look at how to stop this after one pass
    // column loop
    for (int i = 0; i < 11; i++)
    {
        // row loop
        for (int j = 0; j < 11; j++)
        {
            // x positions are snapped to 40px columns
            int x = i * 40;

            // this number sweeps back and forth from -1 to 1
            float osc1 = sinf(time + i * 0.3f);
            // this number also sweeps back and forth, but at a different rate
            float osc2 = sinf(time / 4 + j * 0.09f);

            // y positions are influenced by one of the sweepy numbers
            int y = j * 40 + osc1 * 10;

            // the circles radius are influenced by the other sweepy number
            getPlaydateAPI()->graphics->fillRect(x - (osc2 * 40), y - (osc2 * 40), osc2 * 40 * 2, osc2 * 40 * 2, kColorBlack);
        }
    }
}
