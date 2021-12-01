/*
 * File: MBSS_computeAngularSpectrum.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 17-Nov-2021 10:34:49
 */

/* Include Files */
#include "MBSS_computeAngularSpectrum.h"
#include "main5_data.h"
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
 * Arguments    : const double alpha[604920]
 *                const cell_wrap_2 alphaSampled[120]
 *                const cell_wrap_2 tauGrid[120]
 *                const creal_T X[131072]
 *                double specInst[10082]
 * Return Type  : void
 */
void MBSS_computeAngularSpectrum(const double alpha[604920],
                                 const cell_wrap_2 alphaSampled[120],
                                 const cell_wrap_2 tauGrid[120],
                                 const creal_T X[131072],
                                 double specInst[10082])
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
  emxArray_real_T *specSampledgrid;
  emxArray_real_T *x;
  emxArray_real_T *y;
  creal_T hatRxx[2048];
  creal_T EXP[512];
  creal_T b_y[256];
  double specCurrentPair[10082];
  double NUM[512];
  double TR[512];
  double b_b[512];
  double SINC[256];
  double a[256];
  double im;
  double r;
  double re;
  double y_im;
  double y_re;
  int b_i;
  int c_i;
  int i;
  int ibtile;
  int k;
  int mid_i;
  int npages;
  int pkInd;
  signed char hatRxx_tmp[2];
  /*  OUTPUT: */
  /*  specInst : nGrid x nFrames, angular spectrum for each frame */
  /*  */
  /*  Version: v2.0 */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   */
  /*  Copyright 2018 Ewen Camberlein and Romain Lebarbenchon */
  /*  This software is distributed under the terms of the GNU Public License */
  /*  version 3 (http://www.gnu.org/licenses/gpl.txt) */
  /*  If you find it useful, please cite the following reference: */
  /*  http://bass-db.gforge.inria.fr/bss_locate/ */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   */
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
  /*  Version : v2.0 */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   */
  /*  Copyright 2018 Ewen Camberlein and Romain Lebarbenchon */
  /*  This software is distributed under the terms of the GNU Public License */
  /*  version 3 (http://www.gnu.org/licenses/gpl.txt) */
  /*  If you find it useful, please cite the following reference: */
  /*  http://bass-db.gforge.inria.fr/bss_locate/ */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   */
  /*  Computing the angular spectrum */
  /*  local spectrum */
  /*  nbin x nFrames x 2 x 2 */
  memset(&specInst[0], 0, 10082U * sizeof(double));
  emxInit_real_T(&specSampledgrid, 2);
  emxInit_real_T(&SNR, 3);
  emxInit_real_T(&y, 3);
  emxInit_real_T(&b, 2);
  emxInit_real_T(&x, 1);
  for (i = 0; i < 120; i++) {
    hatRxx_tmp[0] = pairId[i];
    hatRxx_tmp[1] = pairId[i + 120];
    for (b_i = 0; b_i < 2; b_i++) {
      for (c_i = 0; c_i < 2; c_i++) {
        for (mid_i = 0; mid_i < 2; mid_i++) {
          for (npages = 0; npages < 256; npages++) {
            ibtile = npages + (mid_i << 8);
            hatRxx[(ibtile + (c_i << 9)) + (b_i << 10)] =
                X[(ibtile + ((hatRxx_tmp[c_i] - 1) << 9)) +
                  ((hatRxx_tmp[b_i] - 1) << 13)];
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
    for (b_i = 0; b_i < 2; b_i++) {
      for (c_i = 0; c_i < 256; c_i++) {
        ibtile = c_i + (b_i << 8);
        TR[ibtile] = hatRxx[ibtile].re + hatRxx[ibtile + 1536].re;
      }
    }
    for (k = 0; k < 256; k++) {
      re = 3.1415926535897931 * ((187.5 * (double)k + 187.5) * dv[i] / 343.0);
      re = sin(re) / re;
      SINC[k] = re;
    }
    b_i = SNR->size[0] * SNR->size[1] * SNR->size[2];
    SNR->size[0] = 256;
    SNR->size[1] = 2;
    SNR->size[2] = tauGrid[i].f1->size[1];
    emxEnsureCapacity_real_T(SNR, b_i);
    ibtile = tauGrid[i].f1->size[1] << 9;
    for (b_i = 0; b_i < ibtile; b_i++) {
      SNR->data[b_i] = 0.0;
    }
    b_i = tauGrid[i].f1->size[1];
    for (pkInd = 0; pkInd < b_i; pkInd++) {
      y_re = tauGrid[i].f1->data[pkInd] * -0.0;
      y_im = tauGrid[i].f1->data[pkInd] * -6.2831853071795862;
      for (k = 0; k < 256; k++) {
        im = 93.75 * (double)k + 93.75;
        re = im * y_re;
        im *= y_im;
        b_y[k].re = re;
        b_y[k].im = im;
        if (im == 0.0) {
          b_y[k].re = exp(re);
          b_y[k].im = 0.0;
        } else {
          r = exp(re / 2.0);
          b_y[k].re = r * (r * cos(im));
          b_y[k].im = r * (r * sin(im));
        }
      }
      for (mid_i = 0; mid_i < 2; mid_i++) {
        ibtile = mid_i << 8;
        for (k = 0; k < 256; k++) {
          EXP[ibtile + k] = b_y[k];
          npages = k + (mid_i << 8);
          re = hatRxx[npages + 1024].re;
          im = hatRxx[npages + 1024].im;
          NUM[npages] =
              ((hatRxx[npages].re * hatRxx[npages + 1536].re -
                hatRxx[npages].im * hatRxx[npages + 1536].im) -
               (re * hatRxx[npages + 512].re - im * hatRxx[npages + 512].im)) /
              (TR[npages] - 2.0 * (re * EXP[npages].re - im * EXP[npages].im));
        }
      }
      for (c_i = 0; c_i < 256; c_i++) {
        a[c_i] = -(SINC[c_i] + 1.0) / 2.0;
      }
      for (mid_i = 0; mid_i < 2; mid_i++) {
        ibtile = mid_i << 8;
        memcpy(&b_b[ibtile], &a[0], 256U * sizeof(double));
      }
      for (c_i = 0; c_i < 256; c_i++) {
        a[c_i] = (1.0 - SINC[c_i]) / 2.0;
      }
      for (mid_i = 0; mid_i < 2; mid_i++) {
        for (k = 0; k < 256; k++) {
          c_i = k + (mid_i << 8);
          re = NUM[c_i];
          SNR->data[(k + 256 * mid_i) + 512 * pkInd] =
              b_b[c_i] + a[k] * re / (0.5 * TR[c_i] - re);
        }
      }
    }
    /*  */
    /*  sum on frequencies */
    if (SNR->size[2] == 0) {
      y->size[0] = 1;
      y->size[1] = 2;
      y->size[2] = 0;
    } else {
      npages = 1;
      k = 3;
      if (SNR->size[2] == 1) {
        k = 2;
      }
      for (mid_i = 2; mid_i <= k; mid_i++) {
        npages *= SNR->size[mid_i - 1];
      }
      b_i = y->size[0] * y->size[1] * y->size[2];
      y->size[0] = 1;
      y->size[1] = 2;
      y->size[2] = SNR->size[2];
      emxEnsureCapacity_real_T(y, b_i);
      for (mid_i = 0; mid_i < npages; mid_i++) {
        ibtile = mid_i << 8;
        y->data[mid_i] = SNR->data[ibtile];
        for (k = 0; k < 255; k++) {
          y->data[mid_i] += SNR->data[(ibtile + k) + 1];
        }
      }
    }
    if (y->size[2] == 0) {
      b->size[0] = 2;
      b->size[1] = 0;
    } else {
      b_i = b->size[0] * b->size[1];
      b->size[0] = 2;
      b->size[1] = y->size[2];
      emxEnsureCapacity_real_T(b, b_i);
      b_i = y->size[2];
      for (k = 0; k < b_i; k++) {
        b->data[2 * k] = y->data[2 * k];
        c_i = 2 * k + 1;
        b->data[c_i] = y->data[c_i];
      }
    }
    b_i = specSampledgrid->size[0] * specSampledgrid->size[1];
    specSampledgrid->size[0] = b->size[1];
    specSampledgrid->size[1] = 2;
    emxEnsureCapacity_real_T(specSampledgrid, b_i);
    ibtile = b->size[1];
    for (b_i = 0; b_i < 2; b_i++) {
      for (c_i = 0; c_i < ibtile; c_i++) {
        specSampledgrid->data[c_i + specSampledgrid->size[0] * b_i] =
            b->data[b_i + 2 * c_i];
      }
    }
    /*  Order 1 interpolation on the entire grid */
    ibtile = alphaSampled[i].f1->size[1];
    b_i = x->size[0];
    x->size[0] = alphaSampled[i].f1->size[1];
    emxEnsureCapacity_real_T(x, b_i);
    for (b_i = 0; b_i < ibtile; b_i++) {
      x->data[b_i] = alphaSampled[i].f1->data[b_i];
    }
    for (b_i = 0; b_i < 10082; b_i++) {
      specCurrentPair[b_i] = rtNaN;
    }
    for (k = 0; k < 5041; k++) {
      re = alpha[i + 120 * k];
      if ((re >= x->data[0]) && (re <= x->data[x->size[0] - 1])) {
        ibtile = x->size[0];
        npages = 1;
        c_i = 2;
        while (ibtile > c_i) {
          mid_i = (npages >> 1) + (ibtile >> 1);
          if (((npages & 1) == 1) && ((ibtile & 1) == 1)) {
            mid_i++;
          }
          if (re >= x->data[mid_i - 1]) {
            npages = mid_i;
            c_i = mid_i + 1;
          } else {
            ibtile = mid_i;
          }
        }
        im = x->data[npages - 1];
        r = (re - im) / (x->data[npages] - im);
        re = specSampledgrid->data[npages - 1];
        if (re == specSampledgrid->data[npages]) {
          specCurrentPair[k] = specSampledgrid->data[npages - 1];
        } else {
          specCurrentPair[k] =
              (1.0 - r) * re + r * specSampledgrid->data[npages];
        }
        re = specSampledgrid->data[(npages + specSampledgrid->size[0]) - 1];
        if (re == specSampledgrid->data[npages + specSampledgrid->size[0]]) {
          specCurrentPair[k + 5041] = re;
        } else {
          specCurrentPair[k + 5041] =
              (1.0 - r) * re +
              r * specSampledgrid->data[npages + specSampledgrid->size[0]];
        }
      }
    }
    /* Aggregation */
    for (b_i = 0; b_i < 10082; b_i++) {
      specInst[b_i] += specCurrentPair[b_i];
    }
  }
  emxFree_real_T(&x);
  emxFree_real_T(&b);
  emxFree_real_T(&y);
  emxFree_real_T(&SNR);
  emxFree_real_T(&specSampledgrid);
  /*  X is a hatRxx in this case */
}

/*
 * File trailer for MBSS_computeAngularSpectrum.c
 *
 * [EOF]
 */
