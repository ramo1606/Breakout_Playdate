#ifndef STATE_H
#define STATE_H

#include "common.h"

typedef unsigned int (*fnPtr)(void);
typedef unsigned int (*fnPtrFl)(float);
typedef EMode (*fnPtrEMode)(void);

typedef struct
{
    fnPtrEMode getNextState;
    fnPtr init;
    fnPtrFl update;
    fnPtrFl draw;
    fnPtr destroy;
} State;

#endif
