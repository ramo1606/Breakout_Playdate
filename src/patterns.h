#ifndef PATTERNS_H
#define PATTERNS_H

#include "pd_api.h"

LCDPattern BayerDither00 =
{
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither01 =
{
        0x11,
        0x00,
        0x00,
        0x00,
        0x11,
        0x00,
        0x00,
        0x00,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither02 =
{
        0x11,
        0x00,
        0x44,
        0x00,
        0x11,
        0x00,
        0x44,
        0x00,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither03 =
{
        0x55,
        0x00,
        0x44,
        0x00,
        0x55,
        0x00,
        0x44,
        0x00,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither04 =
{
        0x55,
        0x00,
        0x55,
        0x00,
        0x55,
        0x00,
        0x55,
        0x00,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither05 =
{
        0x55,
        0x22,
        0x55,
        0x00,
        0x55,
        0x22,
        0x55,
        0x00,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither06 =
{
        0x55,
        0x22,
        0x55,
        0x88,
        0x55,
        0x22,
        0x55,
        0x88,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither07 =
{
        0x55,
        0xAA,
        0x55,
        0x88,
        0x55,
        0xAA,
        0x55,
        0x88,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither08 =
{
        0x55,
        0xAA,
        0x55,
        0xAA,
        0x55,
        0xAA,
        0x55,
        0xAA,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither09 =
{
        0x77,
        0xAA,
        0x55,
        0xAA,
        0x77,
        0xAA,
        0x55,
        0xAA,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither10 =
{
        0x77,
        0xAA,
        0xDD,
        0xAA,
        0x77,
        0xAA,
        0xDD,
        0xAA,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither11 =
{
        0xFF,
        0xAA,
        0xDD,
        0xAA,
        0xFF,
        0xAA,
        0xDD,
        0xAA,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither12 =
{
        0xFF,
        0xAA,
        0xFF,
        0xAA,
        0xFF,
        0xAA,
        0xFF,
        0xAA,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither13 =
{
        0xFF,
        0xBB,
        0xFF,
        0xAA,
        0xFF,
        0xBB,
        0xFF,
        0xAA,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither14 =
{
        0xFF,
        0xBB,
        0xFF,
        0xEE,
        0xFF,
        0xBB,
        0xFF,
        0xEE,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither15 =
{
        0xFF,
        0xFF,
        0xFF,
        0xEE,
        0xFF,
        0xFF,
        0xFF,
        0xEE,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern BayerDither16 =
{
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
};

LCDPattern* ditheringPatterns[17] =
{
    &BayerDither00,
    &BayerDither01,
    &BayerDither02,
    &BayerDither03,
    &BayerDither04,
    &BayerDither05,
    &BayerDither06,
    &BayerDither07,
    &BayerDither08,
    &BayerDither09,
    &BayerDither10,
    &BayerDither11,
    &BayerDither12,
    &BayerDither13,
    &BayerDither14,
    &BayerDither15,
    &BayerDither16
};

#endif