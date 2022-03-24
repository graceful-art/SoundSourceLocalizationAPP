/*
 * File: MBSS_qstft_multi.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 24-Mar-2022 14:32:31
 */

/* Include Files */
#include "MBSS_qstft_multi.h"
#include "FFTImplementationCallback.h"
#include "rt_nonfinite.h"
#include <math.h>
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
 * Arguments    : creal_T Cx[16896]
 * Return Type  : void
 */
void MBSS_qstft_multi(short b_x[1536],creal_T Cx[16896])
{
  static const double dv[64] = {
      0.024543692606170259, 0.073631077818510776, 0.12271846303085129,
      0.17180584824319181,  0.22089323345553233,  0.26998061866787282,
      0.31906800388021339,  0.36815538909255385,  0.41724277430489443,
      0.46633015951723489,  0.51541754472957546,  0.56450492994191592,
      0.6135923151542565,   0.662679700366597,    0.71176708557893753,
      0.760854470791278,    0.80994185600361857,  0.859029241215959,
      0.9081166264282996,   0.95720401164064006,  1.0062913968529805,
      1.0553787820653211,   1.1044661672776617,   1.1535535524900022,
      1.2026409377023426,   1.2517283229146832,   1.3008157081270237,
      1.3499030933393643,   1.3989904785517047,   1.4480778637640452,
      1.4971652489763858,   1.5462526341887264,   1.5953400194010667,
      1.6444274046134073,   1.6935147898257479,   1.7426021750380885,
      1.7916895602504288,   1.8407769454627694,   1.88986433067511,
      1.9389517158874505,   1.9880391010997909,   2.0371264863121317,
      2.086213871524472,    2.1353012567368124,   2.1843886419491532,
      2.2334760271614935,   2.2825634123738339,   2.3316507975861747,
      2.380738182798515,    2.4298255680108558,   2.4789129532231962,
      2.5280003384355365,   2.5770877236478773,   2.6261751088602177,
      2.675262494072558,    2.7243498792848988,   2.7734372644972392,
      2.82252464970958,     2.8716120349219203,   2.9206994201342606,
      2.9697868053466014,   3.0188741905589418,   3.0679615757712821,
      3.1170489609836229};
  static const double winf[15] = {0.038060233744356631, 0.14644660940672621,
                                  0.30865828381745508,  0.49999999999999994,
                                  0.69134171618254481,  0.85355339059327373,
                                  0.96193976625564337,  1.0,
                                  0.96193976625564337,  0.85355339059327373,
                                  0.69134171618254481,  0.49999999999999994,
                                  0.30865828381745508,  0.14644660940672621,
                                  0.038060233744356631};
  static const double dv1[3] = {0.49999999999999994, 1.0, 0.49999999999999994};
  creal_T X[1056];
  creal_T XX_data[1056];
  creal_T b_tmp_data[1056];
  creal_T local_Cx[256];
  creal_T fframe[64];
  double wei_data[1056];
  double pairId[240];
  double c_tmp_data[66];
  double frame[64];
  double x[64];
  double tmp_data[33];
  double b_data[2];
  double XX_im;
  double XX_re;
  double s_im;
  double s_re;
  int comb[2];
  int Cx_tmp;
  int combj;
  int f;
  int i;
  int icomb;
  int indf_size_idx_1;
  int j;
  int k;
  int loop_ub;
  int nind;
  int nmkpi;
  int t;
  signed char indf_data[33];
  /* %% Errors and warnings %%% */
  /* %% STFT %%% */
  /*  File MBSS_stft_multi.m */
  /*  Multichannel short-time Fourier transform (STFT) using */
  /*  half-overlapping sine windows. */
  /*  */
  /*  X=MBSS_stft_multi(x,wlen) */
  /*  */
  /*  Inputs: */
  /*  x: nchan x nsampl matrix containing nchan time-domain mixture signals */
  /*  with nsampl samples */
  /*  wlen: window length (default: 1024 samples or 64ms at 16 kHz, which is */
  /*  optimal for speech source separation via binary time-frequency masking) */
  /*  */
  /*  Output: */
  /*  X: nbin x nfram x nchan matrix containing the STFT coefficients with nbin
   */
  /*  frequency bins and nfram time frames */
  /*  startSample: nfram x 1 , start sample of each frame */
  /*  endSample: nfram x 1, last sample of each frame */
  /* %% Errors and warnings %%% */
  /* %% Computing STFT coefficients %%% */
  /*  Defining sine window */
  for (k = 0; k < 64; k++) {
    x[k] = sin(dv[k]);
  }
  /*  Start and end sample id of each frames */
  for (i = 0; i < 16; i++) {
    for (t = 0; t < 2; t++) {
      /*  Framing */
      s_re = (double)t * 64.0 / 2.0;
      for (k = 0; k < 64; k++) {
        frame[k] = b_x[i + (((int)(s_re + ((double)k + 1.0)) - 1) << 4)] * x[k];
      }
      /*  FFT */
      c_FFTImplementationCallback_doH(frame, fframe);
      memcpy(&X[i * 66 + t * 33], &fframe[0], 33U * sizeof(creal_T));
    }
  }
  /* %% Computation of local covariances for each pair of microphone %%% */
  /*  winf=hanning(2*8-1); */
  /*  wint=hanning(2*2-1).'; */
  /*  Cx = zeros(nchan,nchan,nbin,nfram); */
  memset(&Cx[0], 0, 16896U * sizeof(creal_T));
  comb[0] = 1;
  comb[1] = 2;
  icomb = 1;
  nmkpi = 16;
  for (i = 0; i < 120; i++) {
    pairId[i] = (double)(comb[0] - 1) + 1.0;
    pairId[i + 120] = (double)(comb[1] - 1) + 1.0;
    if (icomb + 1 > 0) {
      k = comb[icomb];
      combj = comb[icomb] + 1;
      comb[icomb]++;
      if (k + 1 < nmkpi) {
        k = icomb + 2;
        for (j = k; j < 3; j++) {
          combj++;
          comb[1] = combj;
        }
        icomb = 1;
        nmkpi = 16;
      } else {
        icomb--;
        nmkpi--;
      }
    }
  }
  for (f = 0; f < 33; f++) {
    nmkpi = f - 6;
    if (1 > nmkpi) {
      nmkpi = 1;
    }
    icomb = f + 8;
    if (33 < icomb) {
      icomb = 33;
    }
    if (icomb < nmkpi) {
      indf_size_idx_1 = 0;
    } else {
      loop_ub = icomb - nmkpi;
      indf_size_idx_1 = loop_ub + 1;
      for (k = 0; k <= loop_ub; k++) {
        indf_data[k] = (signed char)(nmkpi + k);
      }
    }
    nind = indf_size_idx_1 << 1;
    loop_ub = 16 * nind;
    Cx_tmp = f << 8;
    for (k = 0; k < indf_size_idx_1; k++) {
      tmp_data[k] = winf[(indf_data[k] - f) + 6];
    }
    for (k = 0; k < 16; k++) {
      for (nmkpi = 0; nmkpi < 2; nmkpi++) {
        for (icomb = 0; icomb < indf_size_idx_1; icomb++) {
          b_tmp_data[(icomb + indf_size_idx_1 * nmkpi) +
                     indf_size_idx_1 * 2 * k] =
              X[((indf_data[icomb] + 33 * nmkpi) + 66 * k) - 1];
        }
      }
    }
    for (k = 0; k < nind; k++) {
      for (nmkpi = 0; nmkpi < 16; nmkpi++) {
        XX_data[nmkpi + 16 * k] = b_tmp_data[k + nind * nmkpi];
      }
    }
    for (t = 0; t < 2; t++) {
      b_data[0] = dv1[1 - t];
      b_data[1] = dv1[2 - t];
      for (k = 0; k < 2; k++) {
        for (nmkpi = 0; nmkpi < indf_size_idx_1; nmkpi++) {
          c_tmp_data[nmkpi + indf_size_idx_1 * k] = tmp_data[nmkpi] * b_data[k];
        }
      }
      for (k = 0; k < nind; k++) {
        for (nmkpi = 0; nmkpi < 16; nmkpi++) {
          wei_data[nmkpi + 16 * k] = c_tmp_data[k];
        }
      }
      for (k = 0; k < loop_ub; k++) {
        s_re = wei_data[k];
        b_tmp_data[k].re = s_re * XX_data[k].re;
        b_tmp_data[k].im = s_re * XX_data[k].im;
      }
      for (j = 0; j < 16; j++) {
        combj = j << 4;
        for (i = 0; i < 16; i++) {
          s_re = 0.0;
          s_im = 0.0;
          for (k = 0; k < nind; k++) {
            icomb = k << 4;
            nmkpi = icomb + j;
            XX_re = XX_data[nmkpi].re;
            XX_im = -XX_data[nmkpi].im;
            icomb += i;
            s_re += b_tmp_data[icomb].re * XX_re - b_tmp_data[icomb].im * XX_im;
            s_im += b_tmp_data[icomb].re * XX_im + b_tmp_data[icomb].im * XX_re;
          }
          k = combj + i;
          local_Cx[k].re = s_re;
          local_Cx[k].im = s_im;
        }
      }
      if (nind == 0) {
        XX_im = 0.0;
      } else {
        XX_im = wei_data[0];
        for (k = 2; k <= nind; k++) {
          XX_im += wei_data[16 * (k - 1)];
        }
      }
      for (k = 0; k < 256; k++) {
        s_re = local_Cx[k].re;
        XX_re = local_Cx[k].im;
        if (XX_re == 0.0) {
          s_im = s_re / XX_im;
          s_re = 0.0;
        } else if (s_re == 0.0) {
          s_im = 0.0;
          s_re = XX_re / XX_im;
        } else {
          s_im = s_re / XX_im;
          s_re = XX_re / XX_im;
        }
        local_Cx[k].re = s_im;
        local_Cx[k].im = s_re;
      }
      for (icomb = 0; icomb < 120; icomb++) {
        nmkpi = (int)pairId[icomb];
        k = (int)pairId[icomb + 120];
        i = (nmkpi - 1) << 4;
        Cx[(((nmkpi + i) + Cx_tmp) + 8448 * t) - 1] = local_Cx[(nmkpi + i) - 1];
        Cx[(((k + i) + Cx_tmp) + 8448 * t) - 1] = local_Cx[(k + i) - 1];
        i = (k - 1) << 4;
        Cx[(((nmkpi + i) + Cx_tmp) + 8448 * t) - 1] = local_Cx[(nmkpi + i) - 1];
        Cx[(((k + i) + Cx_tmp) + 8448 * t) - 1] = local_Cx[(k + i) - 1];
      }
    }
  }
}

/*
 * File trailer for MBSS_qstft_multi.c
 *
 * [EOF]
 */
