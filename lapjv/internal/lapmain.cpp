/************************************************************************
*
*  lapmain.cpp
   version 1.0 - 4 September 1996
   author: Roy Jonker @ MagicLogic Optimization Inc.
   
   main program file to run and check Jonker-Volgenant LAP code
*
*************************************************************************/

#include "system.h"
#include "gnrl.h"
#include "lap.h"

  
void main()
{
  #define COSTRANGE 1000.0
  #define PRINTCOST 0

  int dim, startdim, enddim;
  cost **assigncost, *u, *v, lapcost;
  row i, *colsol;
  col j, *rowsol;
  double runtime;

  printf("start dimension ?\n");
  scanf("%d", &startdim);
  printf("end dimension ?\n");
  scanf("%d", &enddim);
  printf("\ndimensions %d .. %d\n", startdim, enddim);

  assigncost = new cost*[enddim];
  for (i = 0; i < enddim; i++)
    assigncost[i] = new cost[enddim];

  rowsol = new col[enddim];
  colsol = new row[enddim];
  u = new cost[enddim];
  v = new cost[enddim];

  for (dim = startdim; dim <= enddim; dim++)
  {
    seedRandom(1000 * dim);
    /* in Visual C++ the first random numbers are not very random.
       call random couple of times before we really start. */
    random(); random(); random(); random(); random(); 
    for (i = 0; i < dim; i++)
      for (j = 0; j < dim; j++)
        assigncost[i][j] = (cost) (random() * (double) COSTRANGE);

#if (PRINTCOST) 
    for (i = 0; i < dim; i++)
    {
      printf("\n");
      for (j = 0; j < dim; j++)
        printf("%4d ", assigncost[i][j]);
    }
#endif
    
    printf("\nstart\n");
    runtime = seconds();
    lapcost = lap(dim, assigncost, rowsol, colsol, u, v);
    runtime = seconds() - runtime;
    printf("\n\ndim  %4d - lap cost %5d - runtime %6.3f\n", dim, lapcost, runtime);
  
    checklap(dim, assigncost, rowsol, colsol, u, v);
  }

  delete[] assigncost;
  delete[] rowsol;
  delete[] colsol;
  delete[] u;
  delete[] v;

  printf("\n\npress key\n");
  char c;
  scanf("%d", &c);
}

