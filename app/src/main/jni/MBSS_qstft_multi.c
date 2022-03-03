/*
 * File: MBSS_qstft_multi.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Mar-2022 13:09:55
 */

/* Include Files */
#include "MBSS_qstft_multi.h"
#include "MBSS_stft_multi.h"
#include "main5_emxutil.h"
#include "main5_types.h"
#include "mtimes.h"
#include "nchoosek.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * File MBSS_qstft_multi.m
 *  Quadratic linear-scale time-frequency transform based on the local
 *  covariance of a STFT with sine windows
 *
 *  [Cx,f]=MBSS_qstft_multi(x,fs,wlen,lf,lt)
 *
 *  Inputs:
 *  x: nsampl x nchan vector containing a multichannel signal
 *  fs: sampling frequency in Hz
 *  wlen: length of the STFT window (must be a power of 2)
 *  lf: half-width of the frequency neighborhood for the computation of
 *  empirical covariance
 *  lt: half-width of the time neighborhood for the computation of empirical
 *  covariance
 *
 *  Output:
 *  Cx: nchan x nchan x nbin x nfram matrix containing the spatial covariance
 *  matrices of the input signal in all time-frequency bins
 *  startSample: nfram x 1 , start sample of each frame
 *  endSample: nfram x 1, last sample of each frame
 *
 *
 * Arguments    : const double x[131072]
 *                creal_T Cx[1573632]
 * Return Type  : void
 */
void MBSS_qstft_multi(const short x[131072], creal_T Cx[1573632])
{
  static creal_T X[98352];
  static const double winf[15] = {0.038060233744356631, 0.14644660940672621,
                                  0.30865828381745508,  0.49999999999999994,
                                  0.69134171618254481,  0.85355339059327373,
                                  0.96193976625564337,  1.0,
                                  0.96193976625564337,  0.85355339059327373,
                                  0.69134171618254481,  0.49999999999999994,
                                  0.30865828381745508,  0.14644660940672621,
                                  0.038060233744356631};
  static const double dv1[3] = {0.49999999999999994, 1.0, 0.49999999999999994};
  static double b_tmp_data[6147];
  emxArray_creal_T *XX;
  emxArray_creal_T *b_XX;
  emxArray_creal_T *r;
  emxArray_real_T *wei;
  creal_T local_Cx[256];
  double tmp_data[2049];
  double pairId[240];
  double dv[16];
  double b_data[3];
  double ai;
  double bsum;
  double re;
  double y;
  int Cx_tmp;
  int f;
  int hi;
  int indf_size_idx_1;
  int k;
  int lastBlockLength;
  int nblocks;
  int nind;
  int t;
  int xblockoffset;
  short indf_data[2049];
  signed char y_data[2049];

  MBSS_stft_multi(x, X);
  /* %% Computation of local covariances for each pair of microphone %%% */
  /*  winf=hanning(2*8-1); */
  /*  wint=hanning(2*2-1).'; */
  /*  Cx = zeros(nchan,nchan,nbin,nfram); */
  memset(&Cx[0], 0, 1573632U * sizeof(creal_T));
  for (lastBlockLength = 0; lastBlockLength < 16; lastBlockLength++) {
    dv[lastBlockLength] = (double)lastBlockLength + 1.0;
  }
  nchoosek(dv, pairId);
  emxInit_real_T(&wei, 2);
  emxInit_creal_T(&XX, 2);
  emxInit_creal_T(&r, 3);
  emxInit_creal_T(&b_XX, 2);
  for (f = 0; f < 2049; f++) {
    hi = f - 6;
    if (1 > hi) {
      hi = 1;
    }
    nind = f + 8;
    if (2049 < nind) {
      nind = 2049;
    }
    if (nind < hi) {
      indf_size_idx_1 = 0;
    } else {
      xblockoffset = nind - hi;
      indf_size_idx_1 = xblockoffset + 1;
      for (lastBlockLength = 0; lastBlockLength <= xblockoffset;
           lastBlockLength++) {
        indf_data[lastBlockLength] = (short)(hi + lastBlockLength);
      }
    }
    Cx_tmp = f << 8;
    for (lastBlockLength = 0; lastBlockLength < indf_size_idx_1;
         lastBlockLength++) {
      tmp_data[lastBlockLength] = winf[(indf_data[lastBlockLength] - f) + 6];
    }
    for (t = 0; t < 3; t++) {
      if (1 > t) {
        nind = 1;
      } else {
        nind = t;
      }
      hi = t + 2;
      if (3 < hi) {
        hi = 3;
      }
      xblockoffset = hi - nind;
      hi = xblockoffset + 1;
      for (lastBlockLength = 0; lastBlockLength <= xblockoffset;
           lastBlockLength++) {
        y_data[lastBlockLength] = (signed char)(nind + lastBlockLength);
      }
      nind = indf_size_idx_1 * (xblockoffset + 1);
      for (lastBlockLength = 0; lastBlockLength < hi; lastBlockLength++) {
        b_data[lastBlockLength] = dv1[y_data[lastBlockLength] - t];
      }
      for (lastBlockLength = 0; lastBlockLength < hi; lastBlockLength++) {
        for (nblocks = 0; nblocks < indf_size_idx_1; nblocks++) {
          b_tmp_data[nblocks + indf_size_idx_1 * lastBlockLength] =
              tmp_data[nblocks] * b_data[lastBlockLength];
        }
      }
      lastBlockLength = wei->size[0] * wei->size[1];
      wei->size[0] = 16;
      wei->size[1] = nind;
      emxEnsureCapacity_real_T(wei, lastBlockLength);
      for (lastBlockLength = 0; lastBlockLength < nind; lastBlockLength++) {
        for (nblocks = 0; nblocks < 16; nblocks++) {
          wei->data[nblocks + 16 * lastBlockLength] =
              b_tmp_data[lastBlockLength];
        }
      }
      lastBlockLength = r->size[0] * r->size[1] * r->size[2];
      r->size[0] = indf_size_idx_1;
      r->size[1] = xblockoffset + 1;
      r->size[2] = 16;
      emxEnsureCapacity_creal_T(r, lastBlockLength);
      for (lastBlockLength = 0; lastBlockLength < 16; lastBlockLength++) {
        for (nblocks = 0; nblocks < hi; nblocks++) {
          for (xblockoffset = 0; xblockoffset < indf_size_idx_1;
               xblockoffset++) {
            r->data[(xblockoffset + r->size[0] * nblocks) +
                    r->size[0] * r->size[1] * lastBlockLength] =
                X[((indf_data[xblockoffset] + 2049 * (y_data[nblocks] - 1)) +
                   6147 * lastBlockLength) -
                  1];
          }
        }
      }
      lastBlockLength = XX->size[0] * XX->size[1];
      XX->size[0] = 16;
      XX->size[1] = nind;
      emxEnsureCapacity_creal_T(XX, lastBlockLength);
      for (lastBlockLength = 0; lastBlockLength < nind; lastBlockLength++) {
        for (nblocks = 0; nblocks < 16; nblocks++) {
          XX->data[nblocks + 16 * lastBlockLength] =
              r->data[lastBlockLength + nind * nblocks];
        }
      }
      if (wei->size[1] == 0) {
        y = 0.0;
      } else {
        if (wei->size[1] <= 1024) {
          nind = wei->size[1];
          lastBlockLength = 0;
          nblocks = 1;
        } else {
          nind = 1024;
          nblocks = wei->size[1] / 1024;
          lastBlockLength = wei->size[1] - (nblocks << 10);
          if (lastBlockLength > 0) {
            nblocks++;
          } else {
            lastBlockLength = 1024;
          }
        }
        y = wei->data[0];
        for (k = 2; k <= nind; k++) {
          y += wei->data[16 * (k - 1)];
        }
        for (nind = 2; nind <= nblocks; nind++) {
          xblockoffset = (nind - 1) << 10;
          bsum = wei->data[16 * xblockoffset];
          if (nind == nblocks) {
            hi = lastBlockLength;
          } else {
            hi = 1024;
          }
          for (k = 2; k <= hi; k++) {
            bsum += wei->data[16 * ((xblockoffset + k) - 1)];
          }
          y += bsum;
        }
      }
      lastBlockLength = b_XX->size[0] * b_XX->size[1];
      b_XX->size[0] = 16;
      b_XX->size[1] = XX->size[1];
      emxEnsureCapacity_creal_T(b_XX, lastBlockLength);
      xblockoffset = 16 * XX->size[1];
      for (lastBlockLength = 0; lastBlockLength < xblockoffset;
           lastBlockLength++) {
        b_XX->data[lastBlockLength].re =
            wei->data[lastBlockLength] * XX->data[lastBlockLength].re;
        b_XX->data[lastBlockLength].im =
            wei->data[lastBlockLength] * XX->data[lastBlockLength].im;
      }
      mtimes(b_XX, XX, local_Cx);
      for (lastBlockLength = 0; lastBlockLength < 256; lastBlockLength++) {
        bsum = local_Cx[lastBlockLength].re;
        ai = local_Cx[lastBlockLength].im;
        if (ai == 0.0) {
          re = bsum / y;
          bsum = 0.0;
        } else if (bsum == 0.0) {
          re = 0.0;
          bsum = ai / y;
        } else {
          re = bsum / y;
          bsum = ai / y;
        }
        local_Cx[lastBlockLength].re = re;
        local_Cx[lastBlockLength].im = bsum;
      }
      for (nblocks = 0; nblocks < 120; nblocks++) {
        nind = (int)pairId[nblocks];
        hi = (int)pairId[nblocks + 120];
        xblockoffset = (nind - 1) << 4;
        Cx[(((nind + xblockoffset) + Cx_tmp) + 524544 * t) - 1] =
            local_Cx[(nind + xblockoffset) - 1];
        Cx[(((hi + xblockoffset) + Cx_tmp) + 524544 * t) - 1] =
            local_Cx[(hi + xblockoffset) - 1];
        xblockoffset = (hi - 1) << 4;
        Cx[(((nind + xblockoffset) + Cx_tmp) + 524544 * t) - 1] =
            local_Cx[(nind + xblockoffset) - 1];
        Cx[(((hi + xblockoffset) + Cx_tmp) + 524544 * t) - 1] =
            local_Cx[(hi + xblockoffset) - 1];
      }
    }
  }
  emxFree_creal_T(&b_XX);
  emxFree_creal_T(&r);
  emxFree_creal_T(&XX);
  emxFree_real_T(&wei);
}

/*
 * File trailer for MBSS_qstft_multi.c
 *
 * [EOF]
 */
