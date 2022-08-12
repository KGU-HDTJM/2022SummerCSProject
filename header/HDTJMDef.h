#pragma once
#include "HDTJMType.h"
#include <stdlib.h>

#if !defined(HDTJMDEF_H)
#define HDTJMDEF_H

#define EPSILON_F 1.19e-07F

#define EqualFloat(a, b) (fabs((a) - (b)) <  EPSILON_F)

#if defined(_WIN64)
// x86_64
#define GetAlignedSize(size) ((size + 7) & ~7)
#else
// x86
#define GetAlignedSize(size) ((size + 3) & ~3)
#endif

#define IsPointer(ptr) ((Word_t)ptr > (Word_t)0x10000)

#define GetBoolean(bValue) ((bValue) = (bValue) && (bValue))

#endif
