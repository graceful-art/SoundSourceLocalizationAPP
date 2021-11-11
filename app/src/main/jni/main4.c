/*
 * File: main4.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 08-Oct-2021 17:43:04
 */

/* Include Files */
#include "main4.h"
#include "MBSS_computeAngularSpectrum.h"
#include "MBSS_locate_spec.h"
#include "MBSS_preprocess.h"
#include "MBSS_qstft_multi.h"
#include "main5_emxutil.h"
#include "main5_types.h"
#include "permute.h"
#include "rt_nonfinite.h"
#include "rt_nonfinite.h"
#include <string.h>
#include <time.h>

#include <android/log.h>

#define LOGD(...) \
    __android_log_print(ANDROID_LOG_DEBUG, "AudioCaptureNative", __VA_ARGS__)

#define L 768//6144  //行

/* Function Definitions */
/*
 * Arguments    : void
 * Return Type  : void
 */
double* main4(char *buffer,int size)
{
    static short dv1[12288];
    short * res = (short*)buffer;

    static creal_T X[131584];
    static creal_T b_X[131072];
    static creal_T dcv[131072];
    static double alpha[3931320];
    static double specInst[65522];
    static double b[32761];
    static double b_dv[32761];
    static double specGlobal[32761];
    cell_wrap_2 alphaSampled[120];
    cell_wrap_2 tauGrid[120];
    double unusedExpr[2];
    double d;
    double d1;
    int b_tmp;
    int ibcol_tmp;
    int itilerow;
    int k;
    boolean_T p;

  /*  原始理想信号 */
  /*  Microphone array parameters */
  /*  Cartesian coordinates of the microphones (in meters) */
  /*  Warning: All microphones are supposed to be omnidirectionnals ! */
  /*  The microphone array is not moving */
  /*  []: all microphones are used */
  /*  Both array and sources are statics => no time stamps */
  /*  MBSS Locate core Parameters */
  /*  localization method */
  /*  Local angular spectrum method {'GCC-PHAT' 'GCC-NONLIN' 'MVDR' 'MVDRW' 'DS'
   * 'DSW' 'DNM'} */
  /*  Pooling method {'max' 'sum'} */
  /*  1: Normalize instantaneous local angular spectra - 0: No normalization */
  /*  Search space */
  /*  Azimuth search boundaries (�) */
  /*  Elevation search boundaries (�) */
  /*  Resolution (�) of the global 3D reference system {azimuth,elevation} */
  /*  Resolution (�) of the 2D reference system defined for each microphone pair
   */
  /*  Multiple sources parameters */
  /*  Number of sources to be detected */
  /*  Minimum angle between peaks */
  /*  Moving sources parameterss */
  /*  Block duration in seconds (default []: one block for the whole signal) */
  /*  Requested block overlap in percent (default []: No overlap) - is
   * internally rounded to suited values */
  /*  Wiener filtering */
  /*  1: Process a Wiener filtering step in order to attenuate / emphasize the
   * provided excerpt signal into the mixture signal. 0: Disable Wiener
   * filtering */
  /*  Wiener filtering mode {'[]' 'Attenuation' 'Emphasis'} */
  /*  Excerpt of the source(s) to be emphasized or attenuated */
  /*  Display results */
  /*  1: Display angular spectrum found and sources directions found - 0: No
   * display */
  /*  Other parameters */
  /*  Speed of sound (m.s-1) - typical value: 343 m.s-1 (assuming 20癈 in the
   * air at sea level) */
  /*  FFT size in seconds (default []: 0.064 sec) */
  /*  Frequency range (pair of values in Hertz) to aggregate the angular
   * spectrum : [] means that all frequencies will be used */
  /*  Debug */
  /*  1: Enable additional plots to debug the angular spectrum aggregation */
  /*  Run the localization */
  /*  Check mandatory params */
  /*  Check mandatory params */
  /*  Check other params */
  /*  Check angularSpectrumMeth */
  /*   'GCC-PHAT','GCC-NONLIN' ,   else choose 0 */
  /*  whole file entire frames */
  /*  whole file entire frames */
  /*  whole file entire frames */
  /*  default value */
  /*  Compute the frequency axis */
  /*  frequency bins used for the aggregative part */
  /*  Compute the search grid */
    for (itilerow = 0; itilerow < 181; itilerow++) {
      ibcol_tmp = itilerow * 181;
      for (k = 0; k < 181; k++) {
        b_tmp = ibcol_tmp + k;
        b[b_tmp] = (((double)k + 1.0) - 1.0) + -180.0;
        b_dv[b_tmp] = (((double)itilerow + 1.0) - 1.0) + -90.0;
      }
    }


  for (ibcol_tmp = 0; ibcol_tmp < L; ibcol_tmp++) {
      itilerow = ibcol_tmp << 4;
      dv1[itilerow] = *(res + ibcol_tmp*18);
      dv1[itilerow + 1] = *(res + ibcol_tmp*18 + 1);
      dv1[itilerow + 2] = *(res + ibcol_tmp*18 + 2);
      dv1[itilerow + 3] = *(res + ibcol_tmp*18 + 3);
      dv1[itilerow + 4] = *(res + ibcol_tmp*18 + 4);
      dv1[itilerow + 5] = *(res + ibcol_tmp*18 + 5);
      dv1[itilerow + 6] = *(res + ibcol_tmp*18 + 6);
      dv1[itilerow + 7] = *(res + ibcol_tmp*18 + 7);
      dv1[itilerow + 8] = *(res + ibcol_tmp*18 + 8);
      dv1[itilerow + 9] = *(res + ibcol_tmp*18 + 9);
      dv1[itilerow + 10] = *(res + ibcol_tmp*18 + 10);
      dv1[itilerow + 11] = *(res + ibcol_tmp*18 + 11);
      dv1[itilerow + 12] = *(res + ibcol_tmp*18 + 12);
      dv1[itilerow + 13] = *(res + ibcol_tmp*18 + 13);
      dv1[itilerow + 14] = *(res + ibcol_tmp*18 + 14);
      dv1[itilerow + 15] = *(res + ibcol_tmp*18 + 15);
  }


  emxInitMatrix_cell_wrap_2(alphaSampled);
  emxInitMatrix_cell_wrap_2(tauGrid);

  /*  all the mics */
  /*  Fill the structure */
  /*  Compute the microphone array centroid */
  /*  */
  /*  Init the  output structure */
  /*  Linear transform */
  MBSS_qstft_multi(dv1, X);
  /*  Output matrix init */
  /*  estimated azimuth in degrees */
  /*  estimated elevation in degrees */
  /*  elapsed time in seconds */
  /*  Create the aggregationParam structure */
  MBSS_preprocess(b, b_dv, alpha, alphaSampled, tauGrid);
  /*  Apply wiener filtering on X if requested */
  /*  Compute the angular spectrum */
   for (itilerow = 0; itilerow < 2; itilerow++) {
     for (ibcol_tmp = 0; ibcol_tmp < 256; ibcol_tmp++) {
       for (k = 0; k < 16; k++) {
         memcpy(&b_X[(itilerow * 65536 + ibcol_tmp * 256) + k * 16],
                &X[((itilerow * 65792 + ibcol_tmp * 256) + k * 16) + 256],
                16U * sizeof(creal_T));
       }
     }
   }
   permute(b_X, dcv);

    MBSS_computeAngularSpectrum(alpha, alphaSampled, tauGrid, dcv, specInst);
  /*  Normalize instantaneous local angular spectra if requested */
  /*  Pooling */
  emxFreeMatrix_cell_wrap_2(tauGrid);
  emxFreeMatrix_cell_wrap_2(alphaSampled);
   for (itilerow = 0; itilerow < 32761; itilerow++) {
     d = specInst[itilerow];
     specGlobal[itilerow] = d;
     d1 = specInst[itilerow + 32761];
     if (rtIsNaN(d1)) {
       p = false;
     } else if (rtIsNaN(d)) {
       p = true;
     } else {
       p = (d < d1);
     }
     if (p) {
       specGlobal[itilerow] = d1;
     }
   }
  MBSS_findPeaks2D(specGlobal, b, b_dv, unusedExpr);
  /*  Print the result */
  LOGD("%.2f ", unusedExpr[0]);
  LOGD("%.2f ", unusedExpr[1]);
  return specGlobal;
}

/*
 * File trailer for main4.c
 *
 * [EOF]
 */
