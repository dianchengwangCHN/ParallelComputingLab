// Template for Programming Assignment 3
// Use "module load cuda" to enable compilation with the Nvidia C compiler nvcc
// Use "nvcc -O3" to compile code; this can be done even on an OSC login node (does not have a GPU)
// To execute compiled code, you must either use a batch submission to run on a node with GPU
// or obtain an interactive GPU-node by using: qsub -I -l walltime=0:59:00 -l nodes=1:gpus=1 -A PAS1488

#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#define threshold 1e-8
#define n (1024)
// Change n to 1024 for final testing; 
// #define n (256)
// n is set to 256 since execution time of single thread template version is excessive
#define SIZE (32)
void init(void);
void ref(void);
void compare(int N, double *wref, double *w);
__global__ void test_kernel(int N, double *A, double *B, double *C);
double rtclock(void);

double a[n][n],b[n][n],c[n][n],cref[n][n];

int main(){

  double clkbegin, clkend, t;
  double *Ad,*Bd,*Cd;
  int size;

  printf("Matrix Size = %d\n",n);

  init();
  clkbegin = rtclock();
  ref();
  clkend = rtclock();
  t = clkend-clkbegin;
  printf("Seq: Approx GFLOPS: %.1f ; Time = %.3f sec; cref[n/2][n/2-1] = %f; \n",
    2.0*n*n*n/t/1e9,t,cref[n/2][n/2-1]);

  
  size = sizeof(double)*n*n;
  cudaMalloc((void **) &Ad,size);
  cudaMalloc((void **) &Bd,size);
  cudaMalloc((void **) &Cd,size);
  cudaMemcpy(Ad,a,size,cudaMemcpyHostToDevice);
  cudaMemcpy(Bd,b,size,cudaMemcpyHostToDevice);

  dim3 threads(SIZE, SIZE);
  dim3 grid(n / threads.x / 2, n / threads.y / 2);

  clkbegin = rtclock();
  test_kernel<<<grid, threads>>>(n,Ad,Bd,Cd);
  if (cudaDeviceSynchronize() != cudaSuccess) 
    printf ("Error return for test_kernel: Was execution done on a node with a GPU?\n");
  else
  {
   clkend = rtclock();
   t = clkend-clkbegin;
   cudaMemcpy(c,Cd,size,cudaMemcpyDeviceToHost);
   cudaFree(Ad); cudaFree(Bd); cudaFree(Cd);
   printf("GPU: Approx GFLOPS: %.1f ; Time = %.3f sec; c[n/2][n/2-1] = %f; \n",
     2.0*n*n*n/t/1e9,t,c[n/2][n/2-1]);
   printf("Correctness Check for GPU solution:\n");
   compare(n, (double *) c,(double *) cref);
 }
}

__global__ void test_kernel(int N, double *A, double *B, double *C)
{
  int i,j,k;
// Template version uses only one thread, which does all the work
// This must be changed (and the launch parameters) to exploit GPU parallelism
// You can make any changes; only requirement is that correctness test passes

  int tx = threadIdx.x, ty = threadIdx.y;
  i = blockIdx.x * blockDim.x * 2 + tx;
  j = blockIdx.y * blockDim.y * 2 + ty;

  __shared__ double 
    Bb0[SIZE * SIZE], Bb1[SIZE * SIZE],
    Ab0[SIZE * SIZE], Ab1[SIZE * SIZE];


  double sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
  for (int ks = 0; ks < N; ks += SIZE) {
    Bb0[ty * SIZE + tx] = B[(ks + tx) * N + j];
    Bb1[ty * SIZE + tx] = B[(ks + tx) * N + j + SIZE];
    Ab0[ty * SIZE + tx] = A[(ks + ty) * N + i];
    Ab1[ty * SIZE + tx] = A[(ks + ty) * N + i + SIZE];
    __syncthreads();
    for (k = 0; k < SIZE; k++) {
      double Atmp0 = Ab0[k * SIZE + tx], Atmp1 = Ab1[k * SIZE + tx];
      double Btmp0 = Bb0[ty * SIZE + k], Btmp1 = Bb1[ty * SIZE + k];
      sum0 += Atmp0 * Btmp0;  // C[j][i]
      sum1 += Atmp1 * Btmp0;  // C[j][i + SIZE]
      sum2 += Atmp0 * Btmp1;  // C[j + SIZE][i]
      sum3 += Atmp1 * Btmp1;  // C[j + SIZE][i + SIZE]
    }
    __syncthreads();
  }
  C[j * N + i] = sum0;
  C[j * N + SIZE + i] = sum1;
  C[(j + SIZE) * N + i] = sum2;
  C[(j + SIZE) * N + SIZE + i] = sum3;
}

void ref(void)
{
  int i,j,k;

  for (j=0;j<n;j++)
   for (k=0;k<n;k++)
    for (i=0;i<n;i++)
      cref[j][i] += a[k][i]*b[k][j];
  }

  void init(void)
  {
    int i,j;
    for(i=0;i<n;i++)
     for(j=0;j<n;j++) 
       { c[i][j] = 0.0; 
         cref[i][j] = 0.0; 
         a[i][j] = drand48();
         b[i][j] = drand48();
       }
     }

     void compare(int N, double *wref, double *w)
     {
      double maxdiff,this_diff;
      int numdiffs;
      int i,j;
      numdiffs = 0;
      maxdiff = 0;
      for (i=0;i<N;i++)
       for (j=0;j<N;j++)
       {
         this_diff = wref[i*N+j]-w[i*N+j];
         if (this_diff < 0) this_diff = -1.0*this_diff;
         if (this_diff>threshold)
          { numdiffs++;
            if (this_diff > maxdiff) maxdiff=this_diff;
          }
        }
        if (numdiffs > 0)
          printf("%d Diffs found over threshold %f; Max Diff = %f\n",
           numdiffs,threshold,maxdiff);
        else
          printf("No differences found between reference and test versions\n");
      }

      double rtclock(void)
      {
        struct timezone Tzp;
        struct timeval Tp;
        int stat;
        stat = gettimeofday (&Tp, &Tzp);
        if (stat != 0) printf("Error return from gettimeofday: %d",stat);
        return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
      }
