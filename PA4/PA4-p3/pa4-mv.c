#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
#define N 8192
#define Niter 10
#define threshold 0.0000001

//
//

double A[N][N], x[N],temp[N],xx[N],temp1[N];
int main(int argc, char *argv[]) {
  double rtclock();

  void mvseq(int n, double m[][n], double x[n], double y[n]);
  void mvpar(int n, int myid, int nprocs, double m[][n], double x[n], double y[n]);
  void compare(int n, double wref[n], double w[n]);

  double clkbegin, clkend;
  double t, tmax, *tarr;
  double rtclock();
  int i,j,it;
  int myid, nprocs;

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &myid );
  MPI_Comm_size( MPI_COMM_WORLD, &nprocs );
  tarr = malloc(sizeof(double)*nprocs);

  for(i=0;i<N;i++)
   { 
     x[i] = xx[i] = 2.0*(i+1)/(1.0*N);

     for(j=0;j<N;j++)
      A[i][j] = 2.0*((i+j) % N)/(1.0*N*(N-1));
   }

  if (myid == 0) 
  {
   clkbegin = rtclock();
   for(it=0;it<Niter;it++)
    mvseq(N,A,x,temp);
   clkend = rtclock();
   t = clkend-clkbegin;
   if (x[N/2]*x[N/2] < -100.0)
    printf("%f\n",x[N/2]);
   printf("Repeated MV: Sequential Version: Matrix Size = %d; %.2f GFLOPS; Time = %.3f sec; \n",
           N,2.0*1e-9*N*N*Niter/t,t);

  }

  MPI_Barrier(MPI_COMM_WORLD);

  clkbegin = rtclock();
  for(it=0;it<Niter;it++)
    mvpar(N,myid,nprocs,A,xx,temp1);
  clkend = rtclock();
  t = clkend-clkbegin;
  if (xx[N/2]*xx[N/2] < -100.0)
    printf("%f\n",xx[N/2]);
  MPI_Reduce(&t, &tmax, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); 
  if (myid == 0)
  {
   printf("Repeated MV: Parallel Version: Matrix Size = %d; %.2f GFLOPS; Time = %.3f sec; \n",
          N,2.0*1e-9*N*N*Niter/tmax,tmax);
   compare(N,x,xx);
  }
}


void mvseq(int n, double m[][n], double x[n], double y[n])
{ int i,j;
  
  for(i=0;i<n;i++)
    y[i]=0.0;

  for(i=0;i<n;i++)
    for(j=0;j<n;j++)
      y[i] = y[i] + m[i][j]*x[j];

  for(i=0;i<n;i++)
    x[i]=y[i];
}

void mvpar(int n, int myid, int nprocs, double m[][n], double x[n], double y[n])
// Initially identical to reference; make your changes to parallelize this code
{ int i,j;
  int rowsperprocs = n / nprocs;
  
  for(i=myid * rowsperprocs;i<(myid + 1) * rowsperprocs;i++) 
    y[i]=0.0;

  for(i=myid * rowsperprocs;i<(myid + 1) * rowsperprocs;i++)
    for(j=0;j<n;j++)
      y[i] = y[i] + m[i][j]*x[j];
  MPI_Allgather(&y[myid * rowsperprocs], rowsperprocs, MPI_DOUBLE, x, rowsperprocs, MPI_DOUBLE, MPI_COMM_WORLD);
}


double rtclock()
{
  struct timezone Tzp;
  struct timeval Tp;
  int stat;
  stat = gettimeofday (&Tp, &Tzp);
  if (stat != 0) printf("Error return from gettimeofday: %d",stat);
  return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}

void compare(int n, double wref[n], double w[n])
{
double maxdiff,this_diff;
double minw,maxw,minref,maxref;
int numdiffs;
int i;
  numdiffs = 0;
  maxdiff = 0;
  minw = minref = 1.0e9;
  maxw = maxref = -1.0;
  for (i=0;i<n;i++)
    {
     this_diff = wref[i]-w[i];
     if (w[i] < minw) minw = w[i];
     if (w[i] > maxw) maxw = w[i];
     if (wref[i] < minref) minref = wref[i];
     if (wref[i] > maxref) maxref = wref[i];
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
      printf("No differences found between base and test versions\n");
   printf("MinRef = %f; MinPar = %f; MaxRef = %f; MaxPar = %f\n",
          minref,minw,maxref,maxw);
}

