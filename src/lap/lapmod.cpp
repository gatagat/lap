#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lapjv.h"

/** Column-reduction and reduction transfer for a sparse cost matrix.
 */
int_t _ccrrt_sparse(const uint_t n, cost_t *cc, uint_t *ii, uint_t *kk,
                      int_t *free_rows, int_t *x, int_t *y, cost_t *v)
{
    int_t n_free_rows;
    boolean *unique;

    for (uint_t i = 0; i < n; i++) {
        x[i] = -1;
        v[i] = LARGE;
        y[i] = 0;
    }
    for (uint_t i = 0; i < n; i++) {
        for (uint_t k = ii[i]; k < ii[i+1]; k++) {
            const int_t j = kk[k];
            const cost_t c = cc[k];
            if (c < v[j]) {
                v[j] = c;
                y[j] = i;
            }
            PRINTF("i=%d, k=%d, j=%d, c[i,j]=%f, v[j]=%f y[j]=%d\n", i, k, j, c, v[j], y[j]);
        }
    }
    PRINT_COST_ARRAY(v, n);
    PRINT_INDEX_ARRAY(y, n);
    NEW(unique, boolean, n);
    memset(unique, TRUE, n);
    {
        int_t j = n;
        do {
            j--;
            const int_t i = y[j];
            if (x[i] < 0) {
                x[i] = j;
            } else {
                unique[i] = FALSE;
                y[j] = -1;
            }
        } while (j > 0);
    }
    n_free_rows = 0;
    for (uint_t i = 0; i < n; i++) {
        if (x[i] < 0) {
            free_rows[n_free_rows++] = i;
        } else if (unique[i] && (ii[i+1] - ii[i] > 1)) {
            const int_t j = x[i];
            cost_t min = LARGE;
            for (uint_t k = ii[i]; k < ii[i+1]; k++) {
                const int_t j2 = kk[k];
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
int_t _carr_sparse(
    const uint_t n, cost_t *cc, uint_t *ii, uint_t *kk,
    const uint_t n_free_rows,
    int_t *free_rows, int_t *x, int_t *y, cost_t *v)
{
    uint_t current = 0;
    int_t new_free_rows = 0;
    uint_t rr_cnt = 0;
    PRINT_INDEX_ARRAY(x, n);
    PRINT_INDEX_ARRAY(y, n);
    PRINT_COST_ARRAY(v, n);
    PRINT_INDEX_ARRAY(free_rows, n_free_rows);
    while (current < n_free_rows) {
        int_t i0;
        int_t j1, j2;
        cost_t v1, v2, v1_new;
        boolean v1_lowers;

        rr_cnt++;
        PRINTF("current = %d rr_cnt = %d\n", current, rr_cnt);
        const int_t free_i = free_rows[current++];
        if (ii[free_i+1] - ii[free_i] > 0) {
            const uint_t k = ii[free_i];
            j1 = kk[k];
            v1 = cc[k] - v[j1];
        } else {
            j1 = 0;
            v1 = LARGE;
        }
        j2 = -1;
        v2 = LARGE;
        for (uint_t k = ii[free_i]+1; k < ii[free_i+1]; k++) {
            PRINTF("%d = %f %d = %f\n", j1, v1, j2, v2);
            const int_t j = kk[k];
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
        i0 = y[j1];
        v1_new = v[j1] - (v2 - v1);
        v1_lowers = v1_new < v[j1];
        PRINTF("%d %d 1=%d,%f 2=%d,%f v1'=%f(%d,%g) \n", free_i, i0, j1, v1, j2, v2, v1_new, v1_lowers, v[j1] - v1_new);
        if (rr_cnt < current * n) {
            if (v1_lowers) {
                v[j1] = v1_new;
            } else if (i0 >= 0 && j2 >= 0) {
                j1 = j2;
                i0 = y[j2];
            }
            if (i0 >= 0) {
                if (v1_lowers) {
                    free_rows[--current] = i0;
                } else {
                    free_rows[new_free_rows++] = i0;
                }
            }
        } else {
            PRINTF("rr_cnt=%d >= %d (current=%d * n=%d)\n", rr_cnt, current * n, current, n);
            if (i0 >= 0) {
                free_rows[new_free_rows++] = i0;
            }
        }
        x[free_i] = j1;
        y[j1] = free_i;
    }
    return new_free_rows;
}


/** Find columns with minimum d[j] and put them on the SCAN list.
 */
uint_t _find_sparse_1(const uint_t n, uint_t lo, cost_t *d, int_t *cols, int_t *y)
{
    uint_t hi = lo + 1;
    cost_t mind = d[cols[lo]];
    for (uint_t k = hi; k < n; k++) {
        int_t j = cols[k];
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
int_t _find_sparse_2(cost_t *d, int_t *scan, const uint_t n_todo, int_t *todo, boolean *done)
{
    int_t hi = 0;
    cost_t mind = LARGE;
    for (uint_t k = 0; k < n_todo; k++) {
        int_t j = todo[k];
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
int_t _scan_sparse_1(
    const uint_t n, cost_t *cc, uint_t *ii, uint_t *kk,
    uint_t *plo, uint_t *phi,
    cost_t *d, int_t *cols, int_t *pred,
    int_t *y, cost_t *v)
{
    uint_t lo = *plo;
    uint_t hi = *phi;
    cost_t h, cred_ij;

    int_t *rev_kk;
    NEW(rev_kk, int_t, n);

    while (lo != hi) {
        int_t kj;
        int_t j = cols[lo++];
        const int_t i = y[j];
        const cost_t mind = d[j];
        for (uint_t k = 0; k < n; k++) {
            rev_kk[k] = -1;
        }
        for (uint_t k = ii[i]; k < ii[i+1]; k++) {
            const int_t j = kk[k];
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
        for (uint_t k = hi; k < n; k++) {
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
int_t _scan_sparse_2(
    const uint_t n, cost_t *cc, uint_t *ii, uint_t *kk,
    uint_t *plo, uint_t *phi,
    cost_t *d, int_t *pred,
    boolean *done, uint_t *pn_ready, int_t *ready, int_t *scan,
    uint_t *pn_todo, int_t *todo, boolean *added,
    int_t *y, cost_t *v)
{
    uint_t lo = *plo;
    uint_t hi = *phi;
    uint_t n_todo = *pn_todo;
    uint_t n_ready = *pn_ready;
    cost_t h, cred_ij;

    int_t *rev_kk;
    NEW(rev_kk, int_t, n);

    for (uint_t k = 0; k < n; k++) {
        rev_kk[k] = -1;
    }
    while (lo != hi) {
        int_t kj;
        int_t j = scan[lo++];
        const int_t i = y[j];
        ready[n_ready++] = j;
        const cost_t mind = d[j];
        for (uint_t k = ii[i]; k < ii[i+1]; k++) {
            const int_t j = kk[k];
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
        for (uint_t k = 0; k < ii[i+1] - ii[i]; k++) {
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
        for (uint_t k = ii[i]; k < ii[i+1]; k++) {
            const int_t j = kk[k];
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
int_t find_path_sparse_1(
    const uint_t n, cost_t *cc, uint_t *ii, uint_t *kk,
    const int_t start_i,
    int_t *y, cost_t *v,
    int_t *pred)
{
    uint_t lo = 0, hi = 0;
    int_t final_j = -1;
    uint_t n_ready = 0;
    int_t *cols;
    cost_t *d;

    NEW(cols, int_t, n);
    NEW(d, cost_t, n);

    for (uint_t i = 0; i < n; i++) {
        cols[i] = i;
        d[i] = LARGE;
        pred[i] = start_i;
    }
    for (uint_t i = ii[start_i]; i < ii[start_i + 1]; i++) {
        const int_t j = kk[i];
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
            for (uint_t k = lo; k < hi; k++) {
                const int_t j = cols[k];
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
        for (uint_t k = 0; k < n_ready; k++) {
            const int_t j = cols[k];
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
int_t find_path_sparse_2(
    const uint_t n, cost_t *cc, uint_t *ii, uint_t *kk,
    const int_t start_i,
    int_t *y, cost_t *v,
    int_t *pred)
{
    uint_t lo = 0, hi = 0;
    int_t final_j = -1;
    uint_t n_ready = 0;
    uint_t n_todo = (ii[start_i + 1] - ii[start_i]);
    boolean *done, *added;
    int_t *ready, *scan, *todo;
    cost_t *d;

    NEW(done, boolean, n);
    NEW(added, boolean, n);
    NEW(ready, int_t, n);
    NEW(scan, int_t, n);
    NEW(todo, int_t, n);
    NEW(d, cost_t, n);

    memset(done, FALSE, n);
    memset(added, FALSE, n);
    for (uint_t i = 0; i < n; i++) {
        d[i] = LARGE;
        pred[i] = start_i;
    }
    for (uint_t i = ii[start_i]; i < ii[start_i + 1]; i++) {
        const int_t j = kk[i];
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
                for (uint_t j = 0; j < n; j++) {
                    if (!done[j] && y[j] < 0) {
                        final_j = j;
                    }
                }
                ASSERT(final_j != -1);
                break;
            }
            ASSERT(hi > lo);
            for (uint_t k = lo; k < hi; k++) {
                const int_t j = scan[k];
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
        for (uint_t k = 0; k < n_ready; k++) {
            const int_t j = ready[k];
            v[j] += d[j] - mind;
        }
    }

    FREE(done);
    FREE(added);
    FREE(ready);
    FREE(scan);
    FREE(todo);
    FREE(d);

    return final_j;
}


/** Find path using one of the two find_path variants selected based on sparsity.
 */
int_t find_path_sparse_dynamic(
    const uint_t n, cost_t *cc, uint_t *ii, uint_t *kk,
    const int_t start_i,
    int_t *y, cost_t *v,
    int_t *pred)
{
    const uint_t n_i = ii[start_i+1] - ii[start_i];
    // XXX: wouldnt it be better to decide for the whole matrix?
    if (n_i > 0.25 * n) {
        return find_path_sparse_1(n, cc, ii, kk, start_i, y, v, pred);
    } else {
        return find_path_sparse_2(n, cc, ii, kk, start_i, y, v, pred);
    }
}


typedef int_t (*fp_function_t)(
        const uint_t, cost_t *, uint_t *, uint_t *, const int_t, int_t *, cost_t *, int_t *);

fp_function_t get_better_find_path(const uint_t n, uint_t *ii)
{
    const double sparsity = ii[n] / (double)(n * n);
    if (sparsity > 0.25) {
        PRINTF("Using find_path_sparse_1 for sparsity=%f\n", sparsity);
        return find_path_sparse_1;
    } else {
        PRINTF("Using find_path_sparse_2 for sparsity=%f\n", sparsity);
        return find_path_sparse_2;
    }
}


/** Augment for a sparse cost matrix.
 */
int_t _ca_sparse(
    const uint_t n, cost_t *cc, uint_t *ii, uint_t *kk,
    const uint_t n_free_rows,
    int_t *free_rows, int_t *x, int_t *y, cost_t *v,
    int fp_version)
{
    int_t *pred;

    NEW(pred, int_t, n);

    fp_function_t fp;
    switch (fp_version) {
        case FP_1: fp = find_path_sparse_1; break;
        case FP_2: fp = find_path_sparse_2; break;
        case FP_DYNAMIC: fp = get_better_find_path(n, ii); break;
        default: return -2;
    }

    for (int_t *pfree_i = free_rows; pfree_i < free_rows + n_free_rows; pfree_i++) {
        int_t i = -1, j;
        uint_t k = 0;

        PRINTF("looking at free_i=%d\n", *pfree_i);
        j = fp(n, cc, ii, kk, *pfree_i, y, v, pred);
        ASSERT(j >= 0);
        ASSERT(j < n);
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
    const uint_t n, cost_t *cc, uint_t *ii, uint_t *kk,
    int_t *x, int_t *y, fp_t fp_version)
{
    int ret;
    int_t *free_rows;
    cost_t *v;

    NEW(free_rows, int_t, n);
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
