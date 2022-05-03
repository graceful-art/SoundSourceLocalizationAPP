/*
 * File: MBSS_computeAngularSpectrum.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 25-Apr-2022 17:17:50
 */

/* Include Files */
#include "MBSS_computeAngularSpectrum.h"
#include "main5_emxutil.h"
#include "main5_types.h"
#include "rt_nonfinite.h"
#include <math.h>
#include <string.h>

/* Function Definitions */
/*
 * Function MBSS_computeAngularSpectrum
 *
 *  This function call the localization method designed by functionName with
 *  aggregationParam, X and f as static argument + varargin arguments
 *
 *  INPUT (mandatory):
 *  functionName     : string, the multi-channel angular spectrum method
 *  debugMode        : 1 x 1, flag to display local angular spectra
 *  aggregationParam : struct, containning the 2-channel angular spectra
 *                     aggregation parameter. These parameters are the following
 * :
 *    * pairId       : nPairs x 2, All microphone pair indexes
 *    * d            : nPairs x 1, For each pair, distance (in meters)
 *                     between microphones
 *    * alpha        : nPairs x nGrid : Array of angles for each
 *                     microphone pair corresponding to all {azimut, elevation}
 *                     to be tested.
 *    * alphaSampled : 1 x nPairs cell array, each cell element contains the
 *                     uniformly distributed angles to be tested for the
 *                     corresponding pair
 *    * azimuth      : 1 x nAz, vector of azimuth values
 *    * elevation    : 1 x nEl, vector of elevation values
 *    * tauGrid      : 1 x nMicPair cell array, each cell element contains the
 *                     TDOA corresponding to the alphaSampled for each pair
 *    * nGrid        : Size of the global grid
 *    * nPairs       : 1 x 1, number of pairs
 *    * c            : 1 x 1, speed of sound (m/s)
 *  X                : nfreq x nfram x N : N multichannel time-frequency
 * transformed signals OR nfreq x nfram x N x N : spatial covariance matrices in
 * all time-frequency bins f                : nfreq x 1, frequency axis in Hertz
 *  freqBins         : 1 x K containing the index of frequency bins used for the
 * aggregation
 *
 * Arguments    : const double d[120]
 *                const double alpha[434520]
 *                const cell_wrap_2 alphaSampled[120]
 *                const cell_wrap_2 tauGrid[120]
 *                const creal_T X[24576]
 *                const double freqBins_data[]
 *                const int freqBins_size[2]
 *                double specInst[10863]
 * Return Type  : void
 */
void MBSS_computeAngularSpectrum(
    const double d[120], const double alpha[434520],
    const cell_wrap_2 alphaSampled[120], const cell_wrap_2 tauGrid[120],
    const creal_T X[24576], const double freqBins_data[],
    const int freqBins_size[2], double specInst[10863])
{
  static const signed char pairId[240] = {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,
      3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,
      8,  8,  9,  9,  9,  9,  9,  9,  9,  10, 10, 10, 10, 10, 10, 11, 11, 11,
      11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 15, 2,  3,  4,  5,  6,  7,
      8,  9,  10, 11, 12, 13, 14, 15, 16, 3,  4,  5,  6,  7,  8,  9,  10, 11,
      12, 13, 14, 15, 16, 4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
      5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 6,  7,  8,  9,  10, 11,
      12, 13, 14, 15, 16, 7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 8,  9,  10,
      11, 12, 13, 14, 15, 16, 9,  10, 11, 12, 13, 14, 15, 16, 10, 11, 12, 13,
      14, 15, 16, 11, 12, 13, 14, 15, 16, 12, 13, 14, 15, 16, 13, 14, 15, 16,
      14, 15, 16, 15, 16, 16};
  emxArray_real_T *SNR;
  emxArray_real_T *b;
  emxArray_real_T *b_y;
  emxArray_real_T *x;
  emxArray_real_T *y;
  creal_T hatRxx_data[396];
  creal_T EXP_data[99];
  creal_T R11_data[99];
  creal_T R12_data[99];
  creal_T R21_data[99];
  creal_T R22_data[99];
  creal_T a_data[33];
  double yi[10863];
  double NUM_data[99];
  double TR_data[99];
  double b_tmp_data[99];
  double tmp_data[99];
  double SINC_data[33];
  double b_a_data[33];
  double b_d;
  double d1;
  double r;
  double y_im;
  double y_re;
  int R12_size_idx_0;
  int b_i;
  int b_loop_ub;
  int f_size;
  int hatRxx_size_idx_0;
  int high_i;
  int i;
  int i1;
  int i2;
  int ibtile;
  int k;
  int loop_ub;
  int mid_i;
  int npages;
  unsigned short f_data[33];
  signed char hatRxx_tmp[2];
  /*  OUTPUT: */
  /*  specInst : nGrid x nFrames, angular spectrum for each frame */
  /*  */
  /*  */
  /*  Function MVDRW_MULTI */
  /*  */
  /*  Compute the MVDRW_SPEC algorithm for all pairs of microphones. */
  /*  */
  /*  INPUT: */
  /*  aggregationParam: structure (see the description above) */
  /*  hatRxx:           nfreq x nfram x N x N , spatial covariance matrices in
   * all */
  /*                    time-frequency bins */
  /*  f:                1 x nfreq, frequency axis */
  /*  freqBins:         1 x K containing the index of frequency bins used for */
  /*                    the aggregation */
  /*  debugMode:        1 x 1, flag to display local angular spectra */
  /*  */
  /*  OUTPUT: */
  /*  specInst:         1 x nDirection, angular spectrum */
  /*  */
  /*  Computing the angular spectrum */
  /*  local spectrum */
  /*  nbin x nFrames x 2 x 2 */
  memset(&specInst[0], 0, 10863U * sizeof(double));
  emxInit_real_T(&y, 3);
  emxInit_real_T(&b, 2);
  loop_ub = freqBins_size[1];
  ibtile = freqBins_size[1];
  b_loop_ub = freqBins_size[1];
  i = freqBins_size[1];
  emxInit_real_T(&SNR, 3);
  emxInit_real_T(&x, 1);
  emxInit_real_T(&b_y, 2);
  hatRxx_size_idx_0 = freqBins_size[1];
  f_size = freqBins_size[1];
  R12_size_idx_0 = freqBins_size[1];
  for (i1 = 0; i1 < ibtile; i1++) {
    f_data[i1] =
        (unsigned short)((unsigned short)(1500U *
                                          (unsigned short)((int)freqBins_data
                                                               [i1] -
                                                           1)) +
                         1500U);
  }
  for (b_i = 0; b_i < 120; b_i++) {
    hatRxx_tmp[0] = pairId[b_i];
    hatRxx_tmp[1] = pairId[b_i + 120];
    for (i1 = 0; i1 < 2; i1++) {
      for (i2 = 0; i2 < 2; i2++) {
        for (mid_i = 0; mid_i < 3; mid_i++) {
          for (high_i = 0; high_i < loop_ub; high_i++) {
            hatRxx_data[((high_i + hatRxx_size_idx_0 * mid_i) +
                         hatRxx_size_idx_0 * 3 * i2) +
                        hatRxx_size_idx_0 * 3 * 2 * i1] =
                X[((((int)freqBins_data[high_i] + (mid_i << 5)) +
                    96 * (hatRxx_tmp[i2] - 1)) +
                   1536 * (hatRxx_tmp[i1] - 1)) -
                  1];
          }
        }
      }
    }
    /*  MVDRW_SPEC Computes the SNR in all directions using the MVDR beamformer
     */
    /*  and frequency weighting */
    /*  */
    /*  spec = mvdrw_spec(hatRxx, f, d, tauGrid) */
    /*  */
    /*  Inputs: */
    /*  hatRxx : nbin x nFrames x 2 x 2 array containing the spatial covariance
     */
    /*      matrices of the input signal in all time-frequency bins */
    /*  f: nbin x 1 vector containing the center frequency of each frequency bin
     */
    /*      in Hz */
    /*  d: microphone spacing in meters */
    /*  tauGrid: 1 x ngrid vector of possible TDOAs in seconds */
    /*  */
    /*  Output: */
    /*  spec: nbin x nFrames x ngrid array of SNR values */
    /*  */
    for (i1 = 0; i1 < 3; i1++) {
      for (i2 = 0; i2 < b_loop_ub; i2++) {
        npages = i2 + hatRxx_size_idx_0 * i1;
        R11_data[i2 + b_loop_ub * i1] = hatRxx_data[npages];
        ibtile = hatRxx_size_idx_0 * 3 * 2;
        R12_data[i2 + R12_size_idx_0 * i1] = hatRxx_data[npages + ibtile];
        high_i = npages + hatRxx_size_idx_0 * 3;
        R21_data[i2 + R12_size_idx_0 * i1] = hatRxx_data[high_i];
        ibtile += high_i;
        R22_data[i2 + R12_size_idx_0 * i1] = hatRxx_data[ibtile];
        TR_data[i2 + R12_size_idx_0 * i1] =
            hatRxx_data[npages].re + hatRxx_data[ibtile].re;
      }
    }
    for (k = 0; k < f_size; k++) {
      b_d = 2.0 * (double)f_data[k] * d[b_i] / 343.0;
      SINC_data[k] = b_d;
      if (fabs(b_d) < 1.0020841800044864E-292) {
        SINC_data[k] = 1.0;
      } else {
        b_d *= 3.1415926535897931;
        b_d = sin(b_d) / b_d;
        SINC_data[k] = b_d;
      }
    }
    i1 = SNR->size[0] * SNR->size[1] * SNR->size[2];
    SNR->size[0] = i;
    SNR->size[1] = 3;
    SNR->size[2] = tauGrid[b_i].f1->size[1];
    emxEnsureCapacity_real_T(SNR, i1);
    ibtile = i * 3 * tauGrid[b_i].f1->size[1];
    for (i1 = 0; i1 < ibtile; i1++) {
      SNR->data[i1] = 0.0;
    }
    i1 = tauGrid[b_i].f1->size[1];
    for (npages = 0; npages < i1; npages++) {
      y_re = tauGrid[b_i].f1->data[npages] * -0.0;
      y_im = tauGrid[b_i].f1->data[npages] * -6.2831853071795862;
      for (k = 0; k < f_size; k++) {
        i2 = f_data[k];
        b_d = (double)i2 * y_re;
        a_data[k].re = b_d;
        d1 = (double)i2 * y_im;
        a_data[k].im = d1;
        if (d1 == 0.0) {
          b_d = exp(b_d);
          a_data[k].re = b_d;
          a_data[k].im = 0.0;
        } else {
          r = exp(b_d / 2.0);
          a_data[k].re = r * (r * cos(d1));
          d1 = r * (r * sin(d1));
          a_data[k].im = d1;
        }
      }
      for (mid_i = 0; mid_i < 3; mid_i++) {
        ibtile = mid_i * f_size;
        for (k = 0; k < f_size; k++) {
          EXP_data[ibtile + k] = a_data[k];
        }
      }
      ibtile = b_loop_ub * 3;
      for (i2 = 0; i2 < ibtile; i2++) {
        b_d = R12_data[i2].re;
        d1 = R12_data[i2].im;
        NUM_data[i2] = ((R11_data[i2].re * R22_data[i2].re -
                         R11_data[i2].im * R22_data[i2].im) -
                        (b_d * R21_data[i2].re - d1 * R21_data[i2].im)) /
                       (TR_data[i2] -
                        2.0 * (b_d * EXP_data[i2].re - d1 * EXP_data[i2].im));
      }
      for (i2 = 0; i2 < f_size; i2++) {
        b_a_data[i2] = (1.0 - SINC_data[i2]) / 2.0;
      }
      for (mid_i = 0; mid_i < 3; mid_i++) {
        ibtile = mid_i * f_size;
        for (k = 0; k < f_size; k++) {
          tmp_data[ibtile + k] = b_a_data[k];
        }
      }
      for (i2 = 0; i2 < f_size; i2++) {
        b_a_data[i2] = -(SINC_data[i2] + 1.0) / 2.0;
      }
      high_i = (signed char)f_size;
      for (mid_i = 0; mid_i < 3; mid_i++) {
        ibtile = mid_i * f_size;
        for (k = 0; k < f_size; k++) {
          b_tmp_data[ibtile + k] = b_a_data[k];
        }
      }
      for (i2 = 0; i2 < 3; i2++) {
        for (mid_i = 0; mid_i < high_i; mid_i++) {
          b_d = NUM_data[mid_i + b_loop_ub * i2];
          SNR->data[(mid_i + SNR->size[0] * i2) + SNR->size[0] * 3 * npages] =
              b_tmp_data[mid_i + (signed char)f_size * i2] +
              tmp_data[mid_i + (signed char)f_size * i2] * b_d /
                  (0.5 * TR_data[mid_i + R12_size_idx_0 * i2] - b_d);
        }
      }
    }
    /*  */
    /*  sum on frequencies */
    /*  Order 1 interpolation on the entire grid */
    /* Aggregation */
    high_i = SNR->size[0];
    if ((SNR->size[0] == 0) || (SNR->size[2] == 0)) {
      i1 = y->size[0] * y->size[1] * y->size[2];
      y->size[0] = 1;
      y->size[1] = 3;
      y->size[2] = SNR->size[2];
      emxEnsureCapacity_real_T(y, i1);
      ibtile = 3 * SNR->size[2];
      for (i1 = 0; i1 < ibtile; i1++) {
        y->data[i1] = 0.0;
      }
    } else {
      npages = 1;
      k = 3;
      if (SNR->size[2] == 1) {
        k = 2;
      }
      for (mid_i = 2; mid_i <= k; mid_i++) {
        npages *= SNR->size[mid_i - 1];
      }
      i1 = y->size[0] * y->size[1] * y->size[2];
      y->size[0] = 1;
      y->size[1] = 3;
      y->size[2] = SNR->size[2];
      emxEnsureCapacity_real_T(y, i1);
      for (mid_i = 0; mid_i < npages; mid_i++) {
        ibtile = mid_i * SNR->size[0];
        y->data[mid_i] = SNR->data[ibtile];
        for (k = 2; k <= high_i; k++) {
          y->data[mid_i] += SNR->data[(ibtile + k) - 1];
        }
      }
    }
    if (y->size[2] == 0) {
      b->size[0] = 3;
      b->size[1] = 0;
    } else {
      i1 = b->size[0] * b->size[1];
      b->size[0] = 3;
      b->size[1] = y->size[2];
      emxEnsureCapacity_real_T(b, i1);
      i1 = y->size[2];
      for (k = 0; k < i1; k++) {
        b->data[3 * k] = y->data[3 * k];
        i2 = 3 * k + 1;
        b->data[i2] = y->data[i2];
        i2 = 3 * k + 2;
        b->data[i2] = y->data[i2];
      }
    }
    ibtile = alphaSampled[b_i].f1->size[1];
    i1 = x->size[0];
    x->size[0] = alphaSampled[b_i].f1->size[1];
    emxEnsureCapacity_real_T(x, i1);
    for (i1 = 0; i1 < ibtile; i1++) {
      x->data[i1] = alphaSampled[b_i].f1->data[i1];
    }
    i1 = b_y->size[0] * b_y->size[1];
    b_y->size[0] = b->size[1];
    b_y->size[1] = 3;
    emxEnsureCapacity_real_T(b_y, i1);
    ibtile = b->size[1];
    for (i1 = 0; i1 < 3; i1++) {
      for (i2 = 0; i2 < ibtile; i2++) {
        b_y->data[i2 + b_y->size[0] * i1] = b->data[i1 + 3 * i2];
      }
    }
    for (i1 = 0; i1 < 10863; i1++) {
      yi[i1] = rtNaN;
    }
    for (k = 0; k < 3621; k++) {
      b_d = alpha[b_i + 120 * k];
      if ((b_d >= x->data[0]) && (b_d <= x->data[x->size[0] - 1])) {
        high_i = x->size[0];
        ibtile = 1;
        npages = 2;
        while (high_i > npages) {
          mid_i = (ibtile >> 1) + (high_i >> 1);
          if (((ibtile & 1) == 1) && ((high_i & 1) == 1)) {
            mid_i++;
          }
          if (b_d >= x->data[mid_i - 1]) {
            ibtile = mid_i;
            npages = mid_i + 1;
          } else {
            high_i = mid_i;
          }
        }
        y_re = x->data[ibtile - 1];
        r = (b_d - y_re) / (x->data[ibtile] - y_re);
        b_d = b_y->data[ibtile - 1];
        if (b_d == b_y->data[ibtile]) {
          yi[k] = b_y->data[ibtile - 1];
        } else {
          yi[k] = (1.0 - r) * b_d + r * b_y->data[ibtile];
        }
        b_d = b_y->data[(ibtile + b_y->size[0]) - 1];
        if (b_d == b_y->data[ibtile + b_y->size[0]]) {
          yi[k + 3621] = b_y->data[(ibtile + b_y->size[0]) - 1];
        } else {
          yi[k + 3621] = (1.0 - r) * b_d + r * b_y->data[ibtile + b_y->size[0]];
        }
        b_d = b_y->data[(ibtile + b_y->size[0] * 2) - 1];
        if (b_d == b_y->data[ibtile + b_y->size[0] * 2]) {
          yi[k + 7242] = b_d;
        } else {
          yi[k + 7242] =
              (1.0 - r) * b_d + r * b_y->data[ibtile + b_y->size[0] * 2];
        }
      }
    }
    for (i1 = 0; i1 < 10863; i1++) {
      specInst[i1] += yi[i1];
    }
  }
  emxFree_real_T(&b_y);
  emxFree_real_T(&x);
  emxFree_real_T(&b);
  emxFree_real_T(&y);
  emxFree_real_T(&SNR);
  /*  X is a hatRxx in this case */
}

/*
 * File trailer for MBSS_computeAngularSpectrum.c
 *
 * [EOF]
 */
