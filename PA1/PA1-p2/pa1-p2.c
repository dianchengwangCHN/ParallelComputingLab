#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#define N 512
#define Niter 10
#define threshold 0.0000001

double A[N][N][N], B[N][N], BB[N][N];
int main()
{
  double rtclock();
  void pa1p2(int n, double x[n][n][n], double y[n][n]);
  void pa1p2opt(int n, double x[n][n][n], double y[n][n]);
  void compare(int n, double wref[n][n], double w[n][n]);

  double clkbegin, clkend;
  double t;
  double rtclock();

  int i, j, k, it;

  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      for (k = 0; k < N; k++)
        A[i][j][k] = (i + 2.0 * j + 3 * k) / (6.0 * N);

  clkbegin = rtclock();
  for (it = 0; it < Niter; it++)
    pa1p2(N, A, B);
  clkend = rtclock();
  t = clkend - clkbegin;
  if (B[0][N / 2] * B[0][N / 2] < -100.0)
    printf("%f\n", B[0][N / 2]);
  printf("Problem 2 Reference Version: Matrix Size = %d; %.2f GFLOPS; Time = %.3f sec;\n",
         N, 2e-9 * N * N * N * Niter / t, t);
  clkbegin = rtclock();

  for (it = 0; it < Niter; it++)
    pa1p2opt(N, A, BB);
  clkend = rtclock();
  t = clkend - clkbegin;
  printf("Problem 2 Optimized Version: Matrix Size = %d; %.2f GFLOPS; Time = %.3f sec; \n",
         N, 2e-9 * N * N * N * Niter / t, t);
  compare(N, B, BB);
}

void pa1p2(int n, double x[n][n][n], double y[n][n])
{
  int i, j, k;
  double sum;
  for (i = 0; i < n; i++)
    for (k = 0; k < n; k++)
    {
      sum = 0.0;
      for (j = 0; j < n; j++)
        sum += x[i][j][k] * x[i][j][k];
      y[i][k] = sum;
    }
}

void pa1p2opt(int n, double x[n][n][n], double y[n][n])
// Initially identical to reference; make your changes to optimize this code
{
  int i, j, k;
  for (i = 0; i < n; i++)
    for (k = 0; k < n; k++)
      y[i][k] = 0.0;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      for (k = 0; k < n; k += 4)
      {
        y[i][k] += x[i][j][k] * x[i][j][k];
        y[i][k + 1] += x[i][j][k + 1] * x[i][j][k + 1];
        y[i][k + 2] += x[i][j][k + 2] * x[i][j][k + 2];
        y[i][k + 3] += x[i][j][k + 3] * x[i][j][k + 3];
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

void compare(int n, double wref[n][n], double w[n][n])
{
  double maxdiff, this_diff;
  int numdiffs;
  int i, j;
  numdiffs = 0;
  maxdiff = 0;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
    {
      this_diff = wref[i][j] - w[i][j];
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
