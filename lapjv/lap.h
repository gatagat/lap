/************************************************************************
*
*  lap.h
   version 1.0 - 21 june 1996
   author  Roy Jonker, MagicLogic Optimization Inc.
   
   header file for LAP
*
**************************************************************************/

/*************** CONSTANTS  *******************/

  #define BIG 100000

/*************** TYPES      *******************/

  typedef int row;
  typedef int col;
  typedef double cost;

/*************** FUNCTIONS  *******************/

extern double lap(int dim, double **assigncost,
               int *rowsol, int *colsol, double *u, double *v);

extern void checklap(int dim, double **assigncost,
                     int *rowsol, int *colsol, double *u, double *v);

