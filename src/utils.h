#ifndef UTILS_H
#define UTILS_H

#define sign(x)  (signbit(x) ?  -1 : 1)

void UTILS_init();

/* Function to find the middle of three number */
float mid(float a, float b, float c);

/* Random Functions */
int randomInt(int min, int max);
float randomFloat(float min, float max);

#endif
