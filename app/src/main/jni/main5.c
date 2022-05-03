/*
 * File: main5.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 25-Apr-2022 17:17:50
 */

/* Include Files */
#include "main5.h"
#include "FFTImplementationCallback.h"
#include "MBSS_locate_spec.h"
#include "main5_data.h"
#include "main5_initialize.h"
#include "main5_rtwutil.h"
#include "rt_nonfinite.h"
#include "rt_nonfinite.h"
#include <math.h>
#include <string.h>

/* Function Declarations */
static double rt_roundd_snf(double u);

/* Function Definitions */
/*
 * Arguments    : double u
 * Return Type  : double
 */
static double rt_roundd_snf(double u)
{
  double y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }
  return y;
}

/*
 * Arguments    : const double MicData[2048]
 *                const double micPosition[48]
 *                const double freqRange[2]
 *                double Fftmax
 *                double fftTest[128]
 *                double specGlobal_data[]
 *                int specGlobal_size[1]
 *                double coordinate_data[]
 *                int coordinate_size[2]
 * Return Type  : void
 */
void main5(const short MicData[2048], const double micPosition[48],
           const double freqRange[2], double Fftmax, double fftTest[128],
           double specGlobal[3621], double coordinate[2])
{
  creal_T y[128];
  double x[2048];
  double d;
  double fftTestMax;
  int i;
  int idx;
  int k;
  int last_tmp;
  boolean_T exitg1;
  if (!isInitialized_main5) {
    main5_initialize();
  }
  for (i = 0; i < 16; i++) {
    for (idx = 0; idx < 128; idx++) {
      x[idx + (i << 7)] = MicData[i + (idx << 4)];
    }
  }
  c_FFTImplementationCallback_doH(*(double(*)[128]) & x[0], y);
  for (k = 0; k < 128; k++) {
    fftTest[k] = rt_hypotd_snf(y[k].re, y[k].im);
  }
  d = rt_roundd_snf(freqRange[0]);
  if (d < 2.147483648E+9) {
    if (d >= -2.147483648E+9) {
      i = (int)d;
    } else {
      i = MIN_int32_T;
    }
  } else if (d >= 2.147483648E+9) {
    i = MAX_int32_T;
  } else {
    i = 0;
  }
  d = rt_roundd_snf(freqRange[1]);
  if (d < 2.147483648E+9) {
    if (d >= -2.147483648E+9) {
      idx = (int)d;
    } else {
      idx = MIN_int32_T;
    }
  } else if (d >= 2.147483648E+9) {
    idx = MAX_int32_T;
  } else {
    idx = 0;
  }
  if (i > 16777215) {
    i = MAX_int32_T;
  } else if (i <= -16777216) {
    i = MIN_int32_T;
  } else {
    i <<= 7;
  }
  i = (int)rt_roundd_snf((double)i / 96000.0);
  if (idx > 16777215) {
    idx = MAX_int32_T;
  } else if (idx <= -16777216) {
    idx = MIN_int32_T;
  } else {
    idx <<= 7;
  }
  idx = (int)rt_roundd_snf((double)idx / 96000.0);
  if (i > idx) {
    i = 0;
    idx = 0;
  } else {
    i--;
  }
  last_tmp = idx - i;
  if (last_tmp <= 2) {
    if (last_tmp == 1) {
      fftTestMax = fftTest[i];
    } else {
      fftTestMax = fftTest[idx - 1];
      if ((!(fftTest[i] < fftTestMax)) &&
          ((!rtIsNaN(fftTest[i])) || rtIsNaN(fftTestMax))) {
        fftTestMax = fftTest[i];
      }
    }
  } else {
    if (!rtIsNaN(fftTest[i])) {
      idx = 1;
    } else {
      idx = 0;
      k = 2;
      exitg1 = false;
      while ((!exitg1) && (k <= last_tmp)) {
        if (!rtIsNaN(fftTest[(i + k) - 1])) {
          idx = k;
          exitg1 = true;
        } else {
          k++;
        }
      }
    }
    if (idx == 0) {
      fftTestMax = fftTest[i];
    } else {
      fftTestMax = fftTest[(i + idx) - 1];
      idx++;
      for (k = idx; k <= last_tmp; k++) {
        d = fftTest[(i + k) - 1];
        if (fftTestMax < d) {
          fftTestMax = d;
        }
      }
    }
  }
  if (fftTestMax > Fftmax) {
    /*  MBSS Locate core Parameters */
    /*  localization method */
    /*  Local angular spectrum method {'GCC-PHAT' 'GCC-NONLIN' 'MVDR' 'MVDRW'
     * 'DS' 'DSW' 'DNM'} */
    /*  Pooling method {'max' 'sum'} */
    /*  Search space */
    /*  Azimuth search boundaries (�) */
    /*  Elevation search boundaries (�) */
    /*  Resolution (�) of the global 3D reference system {azimuth,elevation} */
    /*  Resolution (�) of the 2D reference system defined for each microphone
     * pair */
    /*  Multiple sources parameters */
    /*  Number of sources to be detected */
    /*  Minimum angle between peaks */
    /*  Other parameters */
    /*  Speed of sound (m.s-1) - typical value: 343 m.s-1 (assuming 20癈 in the
     * air at sea level) */
    /*  Debug */
    /*  1: Enable additional plots to debug the angular spectrum aggregation */
    /*  FFT size in seconds (default []: 0.064 sec) */
    /*  Run the localization */
    MBSS_locate_spec(x, micPosition, freqRange, specGlobal, coordinate);
  } else {
    specGlobal[0] = 0.0;
    coordinate[0] = 0.0;
  }
}

/*
 * File trailer for main5.c
 *
 * [EOF]
 */
