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

#include <stdint.h>

/*************** CONSTANTS  *******************/

  #define BIG 100000

/*************** TYPES      *******************/

  typedef int32_t row;
  typedef row col;
  typedef double cost;

/*************** FUNCTIONS  *******************/

extern double lap(int dim, cost **assigncost,
               row *rowsol, col *colsol, cost *u, cost *v);

extern void checklap(int dim, cost **assigncost,
                     row *rowsol, col *colsol, cost *u, cost *v);

#endif
