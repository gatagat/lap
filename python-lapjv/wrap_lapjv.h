// Tomas Kazmar, 2012, BSD 2-clause license, see LICENSE

#ifndef WRAP_LAPJV_H
#define WRAP_LAPJV_H

#include <stdint.h>

double _solve(int dim, double **assigncost, int32_t *rowsol, int32_t *colsol, double *u, double *v);

#endif
