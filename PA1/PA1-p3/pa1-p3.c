#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#define threshold 0.0000001
#define N 128

double A[N][N][N], C[N][N], B[N][N][N], CC[N][N];

int main()
{
  double rtclock();
  void pa1p3(int n, double x[n][n][n], double y[n][n][n], double z[n][n]);
  void pa1p3opt(int n, double x[n][n][n], double y[n][n][n], double z[n][n]);
  void compare(int n, double wref[n][n], double w[n][n]);

  double clkbegin, clkend;
  double t;
  double rtclock();

  int i, j, k;

  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
    {
      C[i][j] = 0;
      for (k = 0; k < N; k++)
      {
        A[i][j][k] = (i + 2 * j + 3 * k) / (3 * N);
        B[i][j][k] = (i - 2 * j + k) / (3 * N);
      }
    }

  clkbegin = rtclock();
  pa1p3(N, A, B, C);
  clkend = rtclock();
  t = clkend - clkbegin;
  if (C[N / 2][N / 2] * C[N / 2][N / 2] < -100.0)
    printf("%f\n", C[N / 2][N / 2]);
  printf("Problem 3 Reference Version: Tensor Size = %d; %.2f GFLOPS; Time = %.3f sec; \n",
         N, 2.0 * 1e-9 * N * N * N * N / t, t);

  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      CC[i][j] = 0;
  clkbegin = rtclock();
  pa1p3opt(N, A, B, CC);
  clkend = rtclock();
  t = clkend - clkbegin;
  if (CC[N / 2][N / 2] * CC[N / 2][N / 2] < -100.0)
    printf("%f\n", CC[N / 2][N / 2]);
  printf("Problem 3 Optimized Version: Tensor Size = %d; %.2f GFLOPS; Time = %.3f sec; \n",
         N, 2.0 * 1e-9 * N * N * N * N / t, t);

  compare(N, C, CC);
}

void pa1p3(int n, double x[n][n][n], double y[n][n][n], double z[n][n])
{
  int i, j, k, l;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      for (k = 0; k < n; k++)
        for (l = 0; l < n; l++)
          z[l][k] += x[l][i][j] * y[i][j][k];
}

void pa1p3opt(int n, double x[n][n][n], double y[n][n][n], double z[n][n])
// Initially identical to reference; make your changes to optimize this code
{
  int i, j, k, l;
  for (i = 0; i < n; i++)
    for (l = 0; l < n; l++)
      for (j = 0; j < n; j++)
        for (k = 0; k < n; k++)
          z[l][k] += x[l][i][j] * y[i][j][k];
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
