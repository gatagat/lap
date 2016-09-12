// Tomas Kazmar, 2012, BSD 2-clause license, see LICENSE

#include "wrap_lapjv.h"
#include "lap.h"

double _solve(int dim, double **assigncost, int32_t *rowsol, int32_t *colsol, double *u, double *v)
{
	return lap(dim, assigncost, rowsol, colsol, u, v);
}

