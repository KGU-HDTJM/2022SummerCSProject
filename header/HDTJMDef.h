#pragma once
#ifndef HDTJMDEF_H
#define HDTJMDEF_H

#include <math.h>

#include "HDTJMType.h"

#define EPSILON_F 1.19e-07F

#define FloatEqual(a, b) (fabsf((a) - (b)) <  EPSILON_F)

#if defined(_WIN64)
// x86_64
#define GetAlignedSize(size) ((size + 7) & ~7)
#else
// x86
#define GetAlignedSize(size) ((size + 3) & ~3)
#endif
#define GetAlgignedSizeForGPU(size) ((size + 15) & ~ 15)

#define IsPointer(ptr) ((Word_t)ptr >= (Word_t)0x10000)

#define GetBoolean(bValue) (!!(bValue))

#endif
