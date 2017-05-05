#ifndef INT32T_H
#define INT32T_H

#ifdef _MSC_VER

typedef signed __int32 int32_t;
typedef unsigned __int32 uint32_t;

#else
#include <stdint.h>
#endif

#endif
