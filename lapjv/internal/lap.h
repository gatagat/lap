/************************************************************************
*
*  lap.h
   version 1.0 - 21 june 1996
   author  Roy Jonker, MagicLogic Optimization Inc.
   
   header file for LAP
*
**************************************************************************/

#ifndef LAP_H
#define LAP_H

#include "int32t.h"

#define BIG 100000

typedef int32_t row;
typedef int32_t col;
typedef double cost;

extern double lap_internal(int dim,
                           cost **assigncost,
                           row *rowsol, col *colsol,
                           cost *u, cost *v);

#endif
