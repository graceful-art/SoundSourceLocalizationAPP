/*
 * File: FFTImplementationCallback.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 24-Mar-2022 14:32:31
 */

/* Include Files */
#include "FFTImplementationCallback.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const double x[64]
 *                creal_T y[64]
 * Return Type  : void
 */
void c_FFTImplementationCallback_doH(const double x[64], creal_T y[64])
{
  static const creal_T reconVar1[32] = {{
                                            1.0, /* re */
                                            -1.0 /* im */
                                        },
                                        {
                                            0.90198285967043934, /* re */
                                            -0.99518472667219693 /* im */
                                        },
                                        {
                                            0.80490967798387181, /* re */
                                            -0.98078528040323043 /* im */
                                        },
                                        {
                                            0.70971532274553772, /* re */
                                            -0.95694033573220882 /* im */
                                        },
                                        {
                                            0.61731656763491016, /* re */
                                            -0.92387953251128674 /* im */
                                        },
                                        {
                                            0.5286032631740023, /* re */
                                            -0.881921264348355  /* im */
                                        },
                                        {
                                            0.44442976698039782, /* re */
                                            -0.83146961230254524 /* im */
                                        },
                                        {
                                            0.36560671583635451, /* re */
                                            -0.773010453362737   /* im */
                                        },
                                        {
                                            0.29289321881345243, /* re */
                                            -0.70710678118654757 /* im */
                                        },
                                        {
                                            0.226989546637263,   /* re */
                                            -0.63439328416364549 /* im */
                                        },
                                        {
                                            0.16853038769745476, /* re */
                                            -0.55557023301960218 /* im */
                                        },
                                        {
                                            0.11807873565164495, /* re */
                                            -0.47139673682599764 /* im */
                                        },
                                        {
                                            0.076120467488713262, /* re */
                                            -0.38268343236508978  /* im */
                                        },
                                        {
                                            0.043059664267791176, /* re */
                                            -0.29028467725446233  /* im */
                                        },
                                        {
                                            0.019214719596769569, /* re */
                                            -0.19509032201612825  /* im */
                                        },
                                        {
                                            0.0048152733278030713, /* re */
                                            -0.0980171403295606    /* im */
                                        },
                                        {
                                            0.0, /* re */
                                            -0.0 /* im */
                                        },
                                        {
                                            0.0048152733278030713, /* re */
                                            0.0980171403295606     /* im */
                                        },
                                        {
                                            0.019214719596769569, /* re */
                                            0.19509032201612825   /* im */
                                        },
                                        {
                                            0.043059664267791176, /* re */
                                            0.29028467725446233   /* im */
                                        },
                                        {
                                            0.076120467488713262, /* re */
                                            0.38268343236508978   /* im */
                                        },
                                        {
                                            0.11807873565164495, /* re */
                                            0.47139673682599764  /* im */
                                        },
                                        {
                                            0.16853038769745476, /* re */
                                            0.55557023301960218  /* im */
                                        },
                                        {
                                            0.226989546637263,  /* re */
                                            0.63439328416364549 /* im */
                                        },
                                        {
                                            0.29289321881345243, /* re */
                                            0.70710678118654757  /* im */
                                        },
                                        {
                                            0.36560671583635451, /* re */
                                            0.773010453362737    /* im */
                                        },
                                        {
                                            0.44442976698039782, /* re */
                                            0.83146961230254524  /* im */
                                        },
                                        {
                                            0.5286032631740023, /* re */
                                            0.881921264348355   /* im */
                                        },
                                        {
                                            0.61731656763491016, /* re */
                                            0.92387953251128674  /* im */
                                        },
                                        {
                                            0.70971532274553772, /* re */
                                            0.95694033573220882  /* im */
                                        },
                                        {
                                            0.80490967798387181, /* re */
                                            0.98078528040323043  /* im */
                                        },
                                        {
                                            0.90198285967043934, /* re */
                                            0.99518472667219693  /* im */
                                        }};
  static const creal_T reconVar2[32] = {{
                                            1.0, /* re */
                                            1.0  /* im */
                                        },
                                        {
                                            1.0980171403295607, /* re */
                                            0.99518472667219693 /* im */
                                        },
                                        {
                                            1.1950903220161282, /* re */
                                            0.98078528040323043 /* im */
                                        },
                                        {
                                            1.2902846772544623, /* re */
                                            0.95694033573220882 /* im */
                                        },
                                        {
                                            1.3826834323650898, /* re */
                                            0.92387953251128674 /* im */
                                        },
                                        {
                                            1.4713967368259977, /* re */
                                            0.881921264348355   /* im */
                                        },
                                        {
                                            1.5555702330196022, /* re */
                                            0.83146961230254524 /* im */
                                        },
                                        {
                                            1.6343932841636455, /* re */
                                            0.773010453362737   /* im */
                                        },
                                        {
                                            1.7071067811865475, /* re */
                                            0.70710678118654757 /* im */
                                        },
                                        {
                                            1.7730104533627369, /* re */
                                            0.63439328416364549 /* im */
                                        },
                                        {
                                            1.8314696123025453, /* re */
                                            0.55557023301960218 /* im */
                                        },
                                        {
                                            1.8819212643483549, /* re */
                                            0.47139673682599764 /* im */
                                        },
                                        {
                                            1.9238795325112867, /* re */
                                            0.38268343236508978 /* im */
                                        },
                                        {
                                            1.9569403357322088, /* re */
                                            0.29028467725446233 /* im */
                                        },
                                        {
                                            1.9807852804032304, /* re */
                                            0.19509032201612825 /* im */
                                        },
                                        {
                                            1.9951847266721969, /* re */
                                            0.0980171403295606  /* im */
                                        },
                                        {
                                            2.0, /* re */
                                            0.0  /* im */
                                        },
                                        {
                                            1.9951847266721969, /* re */
                                            -0.0980171403295606 /* im */
                                        },
                                        {
                                            1.9807852804032304,  /* re */
                                            -0.19509032201612825 /* im */
                                        },
                                        {
                                            1.9569403357322088,  /* re */
                                            -0.29028467725446233 /* im */
                                        },
                                        {
                                            1.9238795325112867,  /* re */
                                            -0.38268343236508978 /* im */
                                        },
                                        {
                                            1.8819212643483549,  /* re */
                                            -0.47139673682599764 /* im */
                                        },
                                        {
                                            1.8314696123025453,  /* re */
                                            -0.55557023301960218 /* im */
                                        },
                                        {
                                            1.7730104533627369,  /* re */
                                            -0.63439328416364549 /* im */
                                        },
                                        {
                                            1.7071067811865475,  /* re */
                                            -0.70710678118654757 /* im */
                                        },
                                        {
                                            1.6343932841636455, /* re */
                                            -0.773010453362737  /* im */
                                        },
                                        {
                                            1.5555702330196022,  /* re */
                                            -0.83146961230254524 /* im */
                                        },
                                        {
                                            1.4713967368259977, /* re */
                                            -0.881921264348355  /* im */
                                        },
                                        {
                                            1.3826834323650898,  /* re */
                                            -0.92387953251128674 /* im */
                                        },
                                        {
                                            1.2902846772544623,  /* re */
                                            -0.95694033573220882 /* im */
                                        },
                                        {
                                            1.1950903220161282,  /* re */
                                            -0.98078528040323043 /* im */
                                        },
                                        {
                                            1.0980171403295607,  /* re */
                                            -0.99518472667219693 /* im */
                                        }};
  static const double dv[16] = {1.0,
                                0.98078528040323043,
                                0.92387953251128674,
                                0.83146961230254524,
                                0.70710678118654757,
                                0.55557023301960218,
                                0.38268343236508978,
                                0.19509032201612825,
                                0.0,
                                -0.19509032201612825,
                                -0.38268343236508978,
                                -0.55557023301960218,
                                -0.70710678118654757,
                                -0.83146961230254524,
                                -0.92387953251128674,
                                -0.98078528040323043};
  static const double dv1[16] = {0.0,
                                 -0.19509032201612825,
                                 -0.38268343236508978,
                                 -0.55557023301960218,
                                 -0.70710678118654757,
                                 -0.83146961230254524,
                                 -0.92387953251128674,
                                 -0.98078528040323043,
                                 -1.0,
                                 -0.98078528040323043,
                                 -0.92387953251128674,
                                 -0.83146961230254524,
                                 -0.70710678118654757,
                                 -0.55557023301960218,
                                 -0.38268343236508978,
                                 -0.19509032201612825};
  static const signed char bitrevIndex[32] = {
      1, 17, 9,  25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31,
      2, 18, 10, 26, 6, 22, 14, 30, 4, 20, 12, 28, 8, 24, 16, 32};
  static const signed char iv[32] = {1,  32, 31, 30, 29, 28, 27, 26, 25, 24, 23,
                                     22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12,
                                     11, 10, 9,  8,  7,  6,  5,  4,  3,  2};
  double temp2_im;
  double temp2_re;
  double temp_im;
  double temp_re;
  double y_im;
  int i;
  int iDelta;
  int iDelta2;
  int iheight;
  int ihi;
  int istart;
  int j;
  int k;
  int temp_re_tmp;
  for (i = 0; i < 32; i++) {
    iDelta = i << 1;
    iDelta2 = bitrevIndex[i] - 1;
    y[iDelta2].re = x[iDelta];
    y[iDelta2].im = x[iDelta + 1];
  }
  for (i = 0; i <= 30; i += 2) {
    temp_re = y[i + 1].re;
    temp_im = y[i + 1].im;
    y[i + 1].re = y[i].re - y[i + 1].re;
    y[i + 1].im = y[i].im - y[i + 1].im;
    y[i].re += temp_re;
    y[i].im += temp_im;
  }
  iDelta = 2;
  iDelta2 = 4;
  k = 8;
  iheight = 29;
  while (k > 0) {
    for (i = 0; i < iheight; i += iDelta2) {
      temp_re_tmp = i + iDelta;
      temp_re = y[temp_re_tmp].re;
      temp_im = y[temp_re_tmp].im;
      y[temp_re_tmp].re = y[i].re - temp_re;
      y[temp_re_tmp].im = y[i].im - temp_im;
      y[i].re += temp_re;
      y[i].im += temp_im;
    }
    istart = 1;
    for (j = k; j < 16; j += k) {
      temp2_re = dv[j];
      temp2_im = dv1[j];
      i = istart;
      ihi = istart + iheight;
      while (i < ihi) {
        temp_re_tmp = i + iDelta;
        temp_re = temp2_re * y[temp_re_tmp].re - temp2_im * y[temp_re_tmp].im;
        temp_im = temp2_re * y[temp_re_tmp].im + temp2_im * y[temp_re_tmp].re;
        y[temp_re_tmp].re = y[i].re - temp_re;
        y[temp_re_tmp].im = y[i].im - temp_im;
        y[i].re += temp_re;
        y[i].im += temp_im;
        i += iDelta2;
      }
      istart++;
    }
    k /= 2;
    iDelta = iDelta2;
    iDelta2 += iDelta2;
    iheight -= iDelta;
  }
  temp2_im = y[0].re;
  temp_im = y[0].im;
  y_im = -y[0].re + y[0].im;
  temp_re = -y[0].im;
  temp2_re = y[0].re - (-y[0].im);
  y[0].re = 0.5 * (temp2_re + temp2_re);
  y[0].im = 0.5 * (y_im + (temp2_im + temp_re));
  temp2_re = temp2_im - temp_im;
  y[32].re = 0.5 * (temp2_re + temp2_re);
  y[32].im = 0.5 * ((temp2_im + temp_im) + (-temp2_im + -temp_im));
  for (i = 0; i < 15; i++) {
    temp_re = y[i + 1].re;
    temp_im = y[i + 1].im;
    iDelta = iv[i + 1];
    temp2_re = y[iDelta - 1].re;
    temp2_im = y[iDelta - 1].im;
    y_im =
        y[i + 1].re * reconVar1[i + 1].im + y[i + 1].im * reconVar1[i + 1].re;
    y[i + 1].re =
        0.5 *
        ((y[i + 1].re * reconVar1[i + 1].re -
          y[i + 1].im * reconVar1[i + 1].im) +
         (temp2_re * reconVar2[i + 1].re - -temp2_im * reconVar2[i + 1].im));
    y[i + 1].im = 0.5 * (y_im + (temp2_re * reconVar2[i + 1].im +
                                 -temp2_im * reconVar2[i + 1].re));
    y[i + 33].re =
        0.5 *
        ((temp_re * reconVar2[i + 1].re - temp_im * reconVar2[i + 1].im) +
         (temp2_re * reconVar1[i + 1].re - -temp2_im * reconVar1[i + 1].im));
    y[i + 33].im =
        0.5 *
        ((temp_re * reconVar2[i + 1].im + temp_im * reconVar2[i + 1].re) +
         (temp2_re * reconVar1[i + 1].im + -temp2_im * reconVar1[i + 1].re));
    y[iDelta - 1].re = 0.5 * ((temp2_re * reconVar1[iDelta - 1].re -
                               temp2_im * reconVar1[iDelta - 1].im) +
                              (temp_re * reconVar2[iDelta - 1].re -
                               -temp_im * reconVar2[iDelta - 1].im));
    y[iDelta - 1].im = 0.5 * ((temp2_re * reconVar1[iDelta - 1].im +
                               temp2_im * reconVar1[iDelta - 1].re) +
                              (temp_re * reconVar2[iDelta - 1].im +
                               -temp_im * reconVar2[iDelta - 1].re));
    y[iDelta + 31].re = 0.5 * ((temp2_re * reconVar2[iDelta - 1].re -
                                temp2_im * reconVar2[iDelta - 1].im) +
                               (temp_re * reconVar1[iDelta - 1].re -
                                -temp_im * reconVar1[iDelta - 1].im));
    y[iDelta + 31].im = 0.5 * ((temp2_re * reconVar2[iDelta - 1].im +
                                temp2_im * reconVar2[iDelta - 1].re) +
                               (temp_re * reconVar1[iDelta - 1].im +
                                -temp_im * reconVar1[iDelta - 1].re));
  }
  temp2_im = y[16].re;
  temp_im = y[16].im;
  temp_re = y[16].re * 0.0;
  temp2_re = y[16].im * 0.0;
  y_im = -y[16].im;
  y[16].re = 0.5 * ((temp_re - temp2_re) + (temp2_im * 2.0 - y_im * 0.0));
  y[16].im = 0.5 * ((temp_re + temp2_re) + (temp2_im * 0.0 + y_im * 2.0));
  temp2_re = -temp_im * 0.0;
  y[48].re =
      0.5 * ((temp2_im * 2.0 - temp_im * 0.0) + (temp2_im * 0.0 - temp2_re));
  y[48].im =
      0.5 * ((temp2_im * 0.0 + temp_im * 2.0) + (temp2_im * 0.0 + temp2_re));
}

/*
 * File trailer for FFTImplementationCallback.c
 *
 * [EOF]
 */
