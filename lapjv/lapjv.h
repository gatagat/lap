#ifndef LAPJV_H
#define LAPJV_H

#include "int32t.h"

typedef int32_t index_t;
typedef double cost_t;

typedef enum { FP_1 = 1, FP_2 = 2, FP_DYNAMIC = 3 } fp_t;

extern index_t lapjv_internal(
    int n, cost_t *cost[],
    index_t *x, index_t *y);

extern index_t lapmod_internal(
    int n, cost_t *cc, index_t *ii, index_t *kk,
    index_t *x, index_t *y, fp_t fp_version);

#endif // LAPJV_H
