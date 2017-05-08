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

/** Column-reduction and reduction transfer for a sparse cost matrix.
 */
index_t _ccrrt_sparse(const index_t n, cost_t *cc, index_t *ii, index_t *kk,
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
        for (size_t k = ii[i]; k < ii[i+1]; k++) {
            const index_t j = kk[k];
            const cost_t c = cc[k];
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
        } else if (unique[i] && (ii[i+1] - ii[i] > 1)) {
            const index_t j = x[i];
            cost_t min = LARGE;
            for (size_t k = ii[i]; k < ii[i+1]; k++) {
                const index_t j2 = kk[k];
                if (j2 == j) {
                    continue;
                }
                const cost_t c = cc[k] - v[j2];
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


/** Augmenting row reduction for a sparse cost matrix.
 */
index_t _carr_sparse(
    const index_t n, cost_t *cc, index_t *ii, index_t *kk,
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
        if (ii[free_i+1] - ii[free_i] > 0) {
            const index_t k = ii[free_i];
            j1 = kk[k];
            v1 = cc[k] - v[j1];
        } else {
            j1 = 0;
            v1 = LARGE;
        }
        j2 = -1;
        v2 = LARGE;
        for (size_t k = ii[free_i]+1; k < ii[free_i+1]; k++) {
            PRINTF("%d = %f %d = %f\n", j1, v1, j2, v2);
            const index_t j = kk[k];
            const cost_t c = cc[k] - v[j];
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
index_t _find_sparse_1(const index_t n, index_t lo, cost_t *d, index_t *cols, index_t *y)
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


/** Find columns with minimum d[j] and put them on the SCAN list.
 */
index_t _find_sparse_2(cost_t *d, index_t *scan, const index_t n_todo, index_t *todo, boolean *done)
{
    index_t hi = 0;
    cost_t mind = LARGE;
    for (size_t k = 0; k < n_todo; k++) {
        index_t j = todo[k];
        if (done[j]) {
            continue;
        }
        if (d[j] <= mind) {
            if (d[j] < mind) {
                hi = 0;
                mind = d[j];
            }
            scan[hi++] = j;
        }
    }
    return hi;
}


/** Scan all columns in TODO starting from arbitrary column in SCAN and try to
 * decrease d of the TODO columns using the SCAN column.
 */
index_t _scan_sparse_1(
    const index_t n, cost_t *cc, index_t *ii, index_t *kk,
    index_t *plo, index_t *phi,
    cost_t *d, index_t *cols, index_t *pred,
    index_t *y, cost_t *v)
{
    index_t lo = *plo;
    index_t hi = *phi;
    cost_t h, cred_ij;

    index_t *rev_kk;
    NEW(rev_kk, index_t, n);

    while (lo != hi) {
        index_t kj;
        index_t j = cols[lo++];
        const index_t i = y[j];
        const cost_t mind = d[j];
        for (size_t k = 0; k < n; k++) {
            rev_kk[k] = -1;
        }
        for (size_t k = ii[i]; k < ii[i+1]; k++) {
            const index_t j = kk[k];
            rev_kk[j] = k;
        }
        PRINTF("?%d kk[%d:%d]=", j, ii[i], ii[i+1]);
        PRINT_INDEX_ARRAY(kk + ii[i], ii[i+1] - ii[i]);
        kj = rev_kk[j];
        if (kj == -1) {
            continue;
        }
        ASSERT(kk[kj] == j);
        h = cc[kj] - v[j] - mind;
        PRINTF("i=%d j=%d kj=%d h=%f\n", i, j, kj, h);
        // For all columns in TODO
        for (size_t k = hi; k < n; k++) {
            j = cols[k];
            PRINTF("?%d kk[%d:%d]=", j, ii[i], ii[i+1]);
            PRINT_INDEX_ARRAY(kk + ii[i], ii[i+1] - ii[i]);
            if ((kj = rev_kk[j]) == -1) {
                continue;
            }
            ASSERT(kk[kj] == j);
            cred_ij = cc[kj] - v[j] - h;
            if (cred_ij < d[j]) {
                d[j] = cred_ij;
                pred[j] = i;
                if (cred_ij == mind) {
                    if (y[j] < 0) {
                        FREE(rev_kk);
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
    FREE(rev_kk);
    return -1;
}


/** Scan all columns in TODO starting from arbitrary column in SCAN and try to
 * decrease d of the TODO columns using the SCAN column.
 */
index_t _scan_sparse_2(
    const index_t n, cost_t *cc, index_t *ii, index_t *kk,
    index_t *plo, index_t *phi,
    cost_t *d, index_t *pred,
    boolean *done, index_t *pn_ready, index_t *ready, index_t *scan,
    index_t *pn_todo, index_t *todo, boolean *added,
    index_t *y, cost_t *v)
{
    index_t lo = *plo;
    index_t hi = *phi;
    index_t n_todo = *pn_todo;
    index_t n_ready = *pn_ready;
    cost_t h, cred_ij;

    index_t *rev_kk;
    NEW(rev_kk, index_t, n);

    for (size_t k = 0; k < n; k++) {
        rev_kk[k] = -1;
    }
    while (lo != hi) {
        index_t kj;
        index_t j = scan[lo++];
        const index_t i = y[j];
        ready[n_ready++] = j;
        const cost_t mind = d[j];
        for (size_t k = ii[i]; k < ii[i+1]; k++) {
            const index_t j = kk[k];
            rev_kk[j] = k;
        }
        PRINTF("?%d kk[%d:%d]=", j, ii[i], ii[i+1]);
        PRINT_INDEX_ARRAY(kk + ii[i], ii[i+1] - ii[i]);
        kj = rev_kk[j];
        ASSERT(kj != -1);
        ASSERT(kk[kj] == j);
        h = cc[kj] - v[j] - mind;
        PRINTF("i=%d j=%d kj=%d h=%f\n", i, j, kj, h);
        // For all columns in TODO
        for (size_t k = 0; k < ii[i+1] - ii[i]; k++) {
            j = kk[ii[i] + k];
            if (done[j]) {
                continue;
            }
            PRINTF("?%d kk[%d:%d]=", j, ii[i], ii[i+1]);
            PRINT_INDEX_ARRAY(kk + ii[i], ii[i+1] - ii[i]);
            cred_ij = cc[ii[i] + k] - v[j] - h;
            if (cred_ij < d[j]) {
                d[j] = cred_ij;
                pred[j] = i;
                if (cred_ij <= mind) {
                    if (y[j] < 0) {
                        FREE(rev_kk);
                        return j;
                    }
                    scan[hi++] = j;
                    done[j] = TRUE;
                } else if (!added[j]) {
                    todo[n_todo++] = j;
                    added[j] = TRUE;
                }
            }
        }
        for (size_t k = ii[i]; k < ii[i+1]; k++) {
            const index_t j = kk[k];
            rev_kk[j] = -1;
        }
    }
    *pn_todo = n_todo;
    *pn_ready = n_ready;
    *plo = lo;
    *phi = hi;
    FREE(rev_kk);
    return -1;
}


/** Single iteration of modified Dijkstra shortest path algorithm as explained in the JV paper.
 *
 * This version loops over all column indices (some of which might be inf).
 *
 * \return The closest free column index.
 */
index_t find_path_sparse_1(
    const index_t n, cost_t *cc, index_t *ii, index_t *kk,
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
        d[i] = LARGE;
        pred[i] = start_i;
    }
    for (size_t i = ii[start_i]; i < ii[start_i + 1]; i++) {
        const index_t j = kk[i];
        d[j] = cc[i] - v[j];
    }
    PRINT_COST_ARRAY(d, n);
    while (final_j == -1) {
        // No columns left on the SCAN list.
        if (lo == hi) {
            PRINTF("%d..%d -> find\n", lo, hi);
            n_ready = lo;
            hi = _find_sparse_1(n, lo, d, cols, y);
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
            final_j = _scan_sparse_1(
                    n, cc, ii, kk, &lo, &hi, d, cols, pred, y, v);
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


/** Single iteration of modified Dijkstra shortest path algorithm as explained in the JV paper.
 *
 * This version loops over non-inf column indices (which requires some additional bookkeeping).
 *
 * \return The closest free column index.
 */
index_t find_path_sparse_2(
    const index_t n, cost_t *cc, index_t *ii, index_t *kk,
    const index_t start_i,
    index_t *y, cost_t *v,
    index_t *pred)
{
    index_t lo = 0, hi = 0, final_j = -1;
    index_t n_ready = 0, n_todo = ii[start_i + 1] - ii[start_i];
    boolean *done, *added;
    index_t *ready, *scan, *todo;
    cost_t *d;

    NEW(done, boolean, n);
    NEW(added, boolean, n);
    NEW(ready, index_t, n);
    NEW(scan, index_t, n);
    NEW(todo, index_t, n);
    NEW(d, cost_t, n);

    memset(done, FALSE, n);
    memset(added, FALSE, n);
    for (size_t i = 0; i < n; i++) {
        d[i] = LARGE;
        pred[i] = start_i;
    }
    for (size_t i = ii[start_i]; i < ii[start_i + 1]; i++) {
        const index_t j = kk[i];
        d[j] = cc[i] - v[j];
        todo[i - ii[start_i]] = j;
        added[j] = TRUE;
    }
    PRINT_COST_ARRAY(d, n);
    PRINT_INDEX_ARRAY(pred, n);
    PRINT_INDEX_ARRAY(done, n);
    PRINT_INDEX_ARRAY(ready, n_ready);
    PRINT_INDEX_ARRAY(scan + lo, hi - lo);
    PRINT_INDEX_ARRAY(todo, n_todo);
    PRINT_INDEX_ARRAY(added, n);
    while (final_j == -1) {
        // No columns left on the SCAN list.
        if (lo == hi) {
            PRINTF("%d..%d -> find\n", lo, hi);
            lo = 0;
            hi = _find_sparse_2(d, scan, n_todo, todo, done);
            PRINTF("check %d..%d\n", lo, hi);
            if (!hi) {
		// XXX: the assignment is unsolvable, lets try to return
		// something reasonable nevertheless.
                for (size_t j = 0; j < n; j++) {
                    if (!done[j] && y[j] < 0) {
                        final_j = j;
                    }
                }
                ASSERT(final_j != -1);
                break;
            }
            ASSERT(hi > lo);
            for (size_t k = lo; k < hi; k++) {
                const index_t j = scan[k];
                if (y[j] < 0) {
                    final_j = j;
                } else {
                    done[j] = TRUE;
                }
            }
        }
        if (final_j == -1) {
            PRINTF("%d..%d -> scan\n", lo, hi);
            PRINT_INDEX_ARRAY(done, n);
            PRINT_INDEX_ARRAY(ready, n_ready);
            PRINT_INDEX_ARRAY(scan + lo, hi - lo);
            PRINT_INDEX_ARRAY(todo, n_todo);
            final_j = _scan_sparse_2(
                    n, cc, ii, kk, &lo, &hi, d, pred,
                    done, &n_ready, ready, scan,
                    &n_todo, todo, added,
                    y, v);
            PRINT_COST_ARRAY(d, n);
            PRINT_INDEX_ARRAY(pred, n);
            PRINT_INDEX_ARRAY(done, n);
            PRINT_INDEX_ARRAY(ready, n_ready);
            PRINT_INDEX_ARRAY(scan + lo, hi - lo);
            PRINT_INDEX_ARRAY(todo, n_todo);
            PRINT_INDEX_ARRAY(added, n);
        }
    }

    PRINTF("found final_j=%d\n", final_j);
    {
        const cost_t mind = d[scan[lo]];
        for (size_t k = 0; k < n_ready; k++) {
            const index_t j = ready[k];
            v[j] += d[j] - mind;
        }
    }

    FREE(scan);
    FREE(d);
    FREE(done);
    FREE(added);
    FREE(todo);

    return final_j;
}


/** Find path using one of the two find_path variants selected based on sparsity.
 */
index_t find_path_sparse_dynamic(
    const index_t n, cost_t *cc, index_t *ii, index_t *kk,
    const index_t start_i,
    index_t *y, cost_t *v,
    index_t *pred)
{
    const index_t n_i = ii[start_i+1] - ii[start_i];
    // XXX: wouldnt it be better to decide for the whole matrix?
    if (n_i > n / 2) {
        return find_path_sparse_1(n, cc, ii, kk, start_i, y, v, pred);
    } else {
        return find_path_sparse_2(n, cc, ii, kk, start_i, y, v, pred);
    }
}


/** Augment for a sparse cost matrix.
 */
index_t _ca_sparse(
    const index_t n, cost_t *cc, index_t *ii, index_t *kk,
    const index_t n_free_rows,
    index_t *free_rows, index_t *x, index_t *y, cost_t *v,
    int fp_version)
{
    index_t *pred;

    NEW(pred, index_t, n);

    index_t (*fp)(const index_t, cost_t *, index_t *, index_t *, const index_t, index_t *, cost_t *, index_t *);
    switch (fp_version) {
        case FP_1: fp = find_path_sparse_1; break;
        case FP_2: fp = find_path_sparse_2; break;
        case FP_DYNAMIC: fp = find_path_sparse_dynamic; break;
        default: return -2;
    }

    for (index_t *pfree_i = free_rows; pfree_i < free_rows + n_free_rows; pfree_i++) {
        index_t i, j, k;

        PRINTF("looking at free_i=%d\n", *pfree_i);
        j = fp(n, cc, ii, kk, *pfree_i, y, v, pred);
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


/** Solve square sparse LAP.
 */
int lapmod_internal(
    const int n, cost_t *cc, index_t *ii, index_t *kk,
    index_t *x, index_t *y, int fp_version)
{
    int ret;
    index_t *free_rows;
    cost_t *v;

    NEW(free_rows, index_t, n);
    NEW(v, cost_t, n);
    ret = _ccrrt_sparse(n, cc, ii, kk, free_rows, x, y, v);
    int i = 0;
    while (ret > 0 && i < 2) {
        ret = _carr_sparse(n, cc, ii, kk, ret, free_rows, x, y, v);
        i++;
    }
    if (ret > 0) {
        ret = _ca_sparse(n, cc, ii, kk, ret, free_rows, x, y, v, fp_version);
    }
    FREE(v);
    FREE(free_rows);
    return ret;
}
