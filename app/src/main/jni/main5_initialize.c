/*
 * File: main5_initialize.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 22-Dec-2021 15:50:26
 */

/* Include Files */
#include "main5_initialize.h"
#include "main5_data.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : void
 * Return Type  : void
 */
void main5_initialize(void)
{
  rt_InitInfAndNaN();
  isInitialized_main5 = true;
}

/*
 * File trailer for main5_initialize.c
 *
 * [EOF]
 */
