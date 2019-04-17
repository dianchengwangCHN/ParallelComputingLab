#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#define N 4096
#define Niter 10
#define threshold 0.0000001

//
// To get vectorization report use the following:
// gcc -O3 -fopt-info filename.c >& filename.gccvecrpt
// icc -O3 -qopt-report=2 -qopt-report-phase=vec filename.c ; Report will be placed in filename.optrpt
//

double A[N][N], x[N], y[N], z[N], yy[N], zz[N];
int main()
{
  double rtclock();

  void pa1p1(int n, double m[][n], double x[n], double y[n], double z[n]);
  void pa1p1opt(int n, double m[][n], double x[n], double y[n], double z[n]);
  void compare(int n, double wref[n], double w[n]);

  double clkbegin, clkend;
  double t;
  double rtclock();

  int i, j, it;

  for (i = 0; i < N; i++)
  {
    x[i] = i;
    y[i] = 0;
    z[i] = 1.0;
    yy[i] = 0;
    zz[i] = 1.0;
    for (j = 0; j < N; j++)
      A[i][j] = (i + 2.0 * j) / (2.0 * N);
  }

  clkbegin = rtclock();
  for (it = 0; it < Niter; it++)
    pa1p1(N, A, x, y, z);
  clkend = rtclock();
  t = clkend - clkbegin;
  if (y[N / 2] * y[N / 2] < -100.0)
    printf("%f\n", y[N / 2]);
  printf("Problem 1 Reference Version: Matrix Size = %d; %.2f GFLOPS; Time = %.3f sec; \n",
         N, 4.0 * 1e-9 * N * N * Niter / t, t);

  for (i = 0; i < N; i++)
  {
    yy[i] = 0;
    zz[i] = 1.0;
  }

  clkbegin = rtclock();
  for (it = 0; it < Niter; it++)
    pa1p1opt(N, A, x, yy, zz);
  clkend = rtclock();
  t = clkend - clkbegin;
  if (yy[N / 2] * yy[N / 2] < -100.0)
    printf("%f\n", yy[N / 2]);
  printf("Problem 1 Optimized Version: Matrix Size = %d; %.2f GFLOPS; Time = %.3f sec; \n",
         N, 4.0 * 1e-9 * N * N * Niter / t, t);
  compare(N, y, yy);
}

void pa1p1(int n, double m[][n], double x[n], double y[n], double z[n])
{
  int i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
    {
      y[j] = y[j] + m[i][j] * x[i];
      z[j] = z[j] + m[j][i] * x[i];
    }
}

void pa1p1opt(int n, double m[][n], double x[n], double y[n], double z[n])
// Initially identical to reference; make your changes to optimize this code
{
  int T = 512;
  int it, jt, i, j;
  // 2D tiling can help reduce
  for (it = 0; it < n; it += T)
    for (jt = 0; jt < n; jt += T)
      for (i = it; i < it + T; i++)
        for (j = jt; j < jt + T; j += 4)
        {
          // Inner loop unrolling
          y[j] = y[j] + m[i][j] * x[i];
          y[j + 1] = y[j + 1] + m[i][j + 1] * x[i];
          y[j + 2] = y[j + 2] + m[i][j + 2] * x[i];
          y[j + 3] = y[j + 3] + m[i][j + 3] * x[i];
          z[i] = z[i] + m[i][j] * x[j];
          z[i] = z[i] + m[i][j + 1] * x[j + 1];
          z[i] = z[i] + m[i][j + 2] * x[j + 2];
          z[i] = z[i] + m[i][j + 3] * x[j + 3];
        }
}

double rtclock()
{
  struct timezone Tzp;
  struct timeval Tp;
  int stat;
  stat = gettimeofday(&Tp, &Tzp);
  if (stat != 0)
    printf("Error return from gettimeofday: %d", stat);
  return (Tp.tv_sec + Tp.tv_usec * 1.0e-6);
}

void compare(int n, double wref[n], double w[n])
{
  double maxdiff, this_diff;
  int numdiffs;
  int i;
  numdiffs = 0;
  maxdiff = 0;
  for (i = 0; i < n; i++)
  {
    this_diff = wref[i] - w[i];
    if (this_diff < 0)
      this_diff = -1.0 * this_diff;
    if (this_diff > threshold)
    {
      numdiffs++;
      if (this_diff > maxdiff)
        maxdiff = this_diff;
    }
  }
  if (numdiffs > 0)
    printf("%d Diffs found over threshold %f; Max Diff = %f\n",
           numdiffs, threshold, maxdiff);
  else
    printf("No differences found between base and test versions\n");
}
