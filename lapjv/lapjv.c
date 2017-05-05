#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined TRUE
#define TRUE 1
#endif
#if !defined FALSE
#define FALSE 0
#endif

typedef char boolean;

#define LARGE 1000000

#include "lapjv.h"

#define NEW(x, t, n) if ((x = (t *)malloc(sizeof(t) * (n))) == 0) { return -1; }
#define FREE(x) if (x != 0) { free(x); x = 0; }

#if 0
#include <assert.h>
#define ASSERT(cond) assert(cond)
#define PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define PRINT_COST_ARRAY(a, n) \
    while (1) { \
        printf(#a" = ["); \
        if ((n) > 0) { \
            printf("%f", (a)[0]); \
            for (size_t j = 1; j < n; j++) { \
                printf(", %f", (a)[j]); \
            } \
        } \
        printf("]\n"); \
        break; \
    }
#define PRINT_INDEX_ARRAY(a, n) \
    while (1) { \
        printf(#a" = ["); \
        if ((n) > 0) { \
            printf("%d", (a)[0]); \
            for (size_t j = 1; j < n; j++) { \
                printf(", %d", (a)[j]); \
            } \
        } \
        printf("]\n"); \
        break; \
    }
#else
#define ASSERT(cond)
#define PRINTF(fmt, ...)
#define PRINT_COST_ARRAY(a, n)
#define PRINT_INDEX_ARRAY(a, n)
#endif

#define SWAP_INDICES(a, b) { index_t _temp_index = a; a = b; b = _temp_index; }

/** Column-reduction and reduction transfer for a dense cost matrix.
 */
index_t _ccrrt_dense(const index_t n, cost_t *cost[],
               index_t *free_rows, index_t *x, index_t *y, cost_t *v)
{
    index_t n_free_rows;
    boolean *unique;

    for (size_t i = 0; i < n; i++) {
        x[i] = -1;
        v[i] = LARGE;
        y[i] = -1;
    }
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            const cost_t c = cost[i][j];
            if (c < v[j]) {
                v[j] = c;
                y[j] = i;
            }
            PRINTF("i=%zu, k=%zu, j=%d, c[i,j]=%f, v[j]=%f y[j]=%d\n", i, k, j, c, v[j], y[j]);
        }
    }
    PRINT_COST_ARRAY(v, n);
    NEW(unique, boolean, n);
    memset(unique, TRUE, n);
    {
        int j = n;
        do {
            j--;
            const index_t i = y[j];
            if (x[i] < 0) {
                x[i] = j;
            } else {
                unique[i] = FALSE;
                y[j] = -1;
            }
        } while (j > 0);
    }
    n_free_rows = 0;
    for (size_t i = 0; i < n; i++) {
        if (x[i] < 0) {
            free_rows[n_free_rows++] = i;
        } else if (unique[i]) {
            const index_t j = x[i];
            cost_t min = LARGE;
            for (size_t j2 = 0; j2 < n; j2++) {
                if (j2 == j) {
                    continue;
                }
                const cost_t c = cost[i][j2] - v[j2];
                if (c < min) {
                    min = c;
                }
            }
            PRINTF("v[%d] = %f - %f\n", j, v[j], min);
            v[j] -= min;
        }
    }
    FREE(unique);
    return n_free_rows;
}


/** Augmenting row reduction for a dense cost matrix.
 */
index_t _carr_dense(
    const index_t n, cost_t *cost[],
    const index_t n_free_rows,
    index_t *free_rows, index_t *x, index_t *y, cost_t *v)
{
    index_t current = 0;
    index_t new_free_rows = 0;
    PRINT_INDEX_ARRAY(x, n);
    PRINT_INDEX_ARRAY(y, n);
    PRINT_COST_ARRAY(v, n);
    PRINT_INDEX_ARRAY(free_rows, n_free_rows);
    while (current < n_free_rows) {
        index_t i0;
        index_t j1, j2;
        cost_t v1, v2, v1_new;
        boolean v1_lowers;

        PRINTF("current = %d\n", current);
        const index_t free_i = free_rows[current++];
        j1 = 0;
        v1 = cost[free_i][0] - v[0];
        j2 = -1;
        v2 = LARGE;
        for (size_t j = 1; j < n; j++) {
            PRINTF("%d = %f %d = %f\n", j1, v1, j2, v2);
            const cost_t c = cost[free_i][j] - v[j];
            if (c < v2) {
                if (c >= v1) {
                    v2 = c;
                    j2 = j;
                } else {
                    v2 = v1;
                    v1 = c;
                    j2 = j1;
                    j1 = j;
                }
            }
        }
        PRINTF("%d = %f %d = %f\n", j1, v1, j2, v2);
        i0 = y[j1];
        v1_new = v[j1] - (v2 - v1);
        v1_lowers = v1_new < v[j1];
        PRINTF("%d %d 1=%d,%f 2=%d,%f %f %d\n", free_i, i0, j1, v1, j2, v2, v1_new, v1_lowers);
        if (v1_lowers) {
            v[j1] = v1_new;
        } else if (i0 >= 0 && j2 >= 0) {
            j1 = j2;
            i0 = y[j2];
        }
        x[free_i] = j1;
        y[j1] = free_i;
        if (i0 >= 0) {
            if (v1_lowers) {
                free_rows[--current] = i0;
            } else {
                free_rows[new_free_rows++] = i0;
            }
        }
    }
    return new_free_rows;
}


/** Find columns with minimum d[j] and put them on the SCAN list.
 */
index_t _find_dense(const index_t n, index_t lo, cost_t *d, index_t *cols, index_t *y)
{
    index_t hi = hi = lo + 1;
    cost_t mind = d[cols[lo]];
    for (size_t k = hi; k < n; k++) {
        index_t j = cols[k];
        if (d[j] <= mind) {
            if (d[j] < mind) {
                hi = lo;
                mind = d[j];
            }
            cols[k] = cols[hi];
            cols[hi++] = j;
        }
    }
    return hi;
}


// Scan all columns in TODO starting from arbitrary column in SCAN
// and try to decrease d of the TODO columns using the SCAN column.
index_t _scan_dense(const index_t n, cost_t *cost[],
                    index_t *plo, index_t *phi,
                    cost_t *d, index_t *cols, index_t *pred,
                    index_t *y, cost_t *v)
{
    index_t lo = *plo;
    index_t hi = *phi;
    cost_t h, cred_ij;

    while (lo != hi) {
        index_t j = cols[lo++];
        const index_t i = y[j];
        const cost_t mind = d[j];
        h = cost[i][j] - v[j] - mind;
        PRINTF("i=%d j=%d kj=%d h=%f\n", i, j, kj, h);
        // For all columns in TODO
        for (size_t k = hi; k < n; k++) {
            j = cols[k];
            cred_ij = cost[i][j] - v[j] - h;
            if (cred_ij < d[j]) {
                d[j] = cred_ij;
                pred[j] = i;
                if (cred_ij == mind) {
                    if (y[j] < 0) {
                        return j;
                    }
                    cols[k] = cols[hi];
                    cols[hi++] = j;
                }
            }
        }
    }
    *plo = lo;
    *phi = hi;
    return -1;
}


/** Single iteration of modified Dijkstra shortest path algorithm as explained in the JV paper.
 *
 * This is a dense matrix version.
 *
 * \return The closest free column index.
 */
index_t find_path_dense(
    const index_t n, cost_t *cost[],
    const index_t start_i,
    index_t *y, cost_t *v,
    index_t *pred)
{
    index_t lo = 0, hi = 0, final_j = -1, n_ready = 0;
    index_t *cols;
    cost_t *d;

    NEW(cols, index_t, n);
    NEW(d, cost_t, n);

    for (size_t i = 0; i < n; i++) {
        cols[i] = i;
        pred[i] = start_i;
        d[i] = cost[start_i][i] - v[i];
    }
    PRINT_COST_ARRAY(d, n);
    while (final_j == -1) {
        // No columns left on the SCAN list.
        if (lo == hi) {
            PRINTF("%d..%d -> find\n", lo, hi);
            n_ready = lo;
            hi = _find_dense(n, lo, d, cols, y);
            PRINTF("check %d..%d\n", lo, hi);
            PRINT_INDEX_ARRAY(cols, n);
            for (size_t k = lo; k < hi; k++) {
                const index_t j = cols[k];
                if (y[j] < 0) {
                    final_j = j;
                }
            }
        }
        if (final_j == -1) {
            PRINTF("%d..%d -> scan\n", lo, hi);
            final_j = _scan_dense(
                    n, cost, &lo, &hi, d, cols, pred, y, v);
            PRINT_COST_ARRAY(d, n);
            PRINT_INDEX_ARRAY(cols, n);
            PRINT_INDEX_ARRAY(pred, n);
        }
    }

    PRINTF("found final_j=%d\n", final_j);
    PRINT_INDEX_ARRAY(cols, n);
    {
        const cost_t mind = d[cols[lo]];
        for (size_t k = 0; k < n_ready; k++) {
            const index_t j = cols[k];
            v[j] += d[j] - mind;
        }
    }

    FREE(cols);
    FREE(d);

    return final_j;
}


/** Augment for a dense cost matrix.
 */
index_t _ca_dense(
    const index_t n, cost_t *cost[],
    const index_t n_free_rows,
    index_t *free_rows, index_t *x, index_t *y, cost_t *v)
{
    index_t *pred;

    NEW(pred, index_t, n);

    for (index_t *pfree_i = free_rows; pfree_i < free_rows + n_free_rows; pfree_i++) {
        index_t i, j, k;

        PRINTF("looking at free_i=%d\n", *pfree_i);
        j = find_path_dense(n, cost, *pfree_i, y, v, pred);
        ASSERT(j >= 0);
        ASSERT(j < n);
        i = -1;
        k = 0;
        while (i != *pfree_i) {
            PRINTF("augment %d\n", j);
            PRINT_INDEX_ARRAY(pred, n);
            i = pred[j];
            PRINTF("y[%d]=%d -> %d\n", j, y[j], i);
            y[j] = i;
            PRINT_INDEX_ARRAY(x, n);
            SWAP_INDICES(j, x[i]);
            k++;
            if (k >= n) {
                ASSERT(FALSE);
            }
        }
    }
    FREE(pred);
    return 0;
}


/** Solve dense sparse LAP.
 */
int lapjv_internal(
    const int n, cost_t *cost[],
    index_t *x, index_t *y)
{
    int ret;
    index_t *free_rows;
    cost_t *v;

    NEW(free_rows, index_t, n);
    NEW(v, cost_t, n);
    ret = _ccrrt_dense(n, cost, free_rows, x, y, v);
    int i = 0;
    while (ret > 0 && i < 2) {
        ret = _carr_dense(n, cost, ret, free_rows, x, y, v);
        i++;
    }
    if (ret > 0) {
        ret = _ca_dense(n, cost, ret, free_rows, x, y, v);
    }
    FREE(v);
    FREE(free_rows);
    return ret;
}
