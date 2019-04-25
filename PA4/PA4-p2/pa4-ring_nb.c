#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

// double in[N*N],out[N*N];
int main(int argc, char *argv[]) {
  double rtclock();

  double clkbegin, clkend;
  double t, tmax, *tarr;
  double rtclock();
  int i,j,it;
  int myid, nprocs;
  int MsgLen,Niter;
  MPI_Status status_in, status_out;
  MPI_Request request_in, request_out;
  double *in,*out;

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &myid );
  MPI_Comm_size( MPI_COMM_WORLD, &nprocs );

  if (myid == 0) 
  {
    printf("Enter Message Length ");
    scanf("%d",&MsgLen);
    printf("Number of spins around ring ");
    scanf("%d",&Niter);
  }
  MPI_Bcast(&MsgLen, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&Niter, 1, MPI_INT, 0, MPI_COMM_WORLD);
  in = (double *) malloc(MsgLen*sizeof(double));
  out = (double *) malloc(MsgLen*sizeof(double));

  for(i=0;i<MsgLen;i++) out[i] = i;

  MPI_Barrier(MPI_COMM_WORLD);

  clkbegin = rtclock();
  for(it=0;it<nprocs*Niter;it++) 
  {
    MPI_Isend(out,MsgLen,MPI_DOUBLE,(myid+1)%nprocs,0,MPI_COMM_WORLD, &request_out);
    MPI_Irecv(in,MsgLen,MPI_DOUBLE,(myid+nprocs-1)%nprocs,0,MPI_COMM_WORLD, &request_in);
    MPI_Wait(&request_in, &status_in);
    MPI_Isend(in,MsgLen,MPI_DOUBLE,(myid+1)%nprocs,0,MPI_COMM_WORLD, &request_in);
    MPI_Wait(&request_out, &status_out);
    MPI_Irecv(out,MsgLen,MPI_DOUBLE,(myid+nprocs-1)%nprocs,0,MPI_COMM_WORLD, &request_out);
    MPI_Wait(&request_out, &status_out);
    MPI_Wait(&request_in, &status_in);
 }
 clkend = rtclock();
 t = clkend-clkbegin;
 MPI_Barrier(MPI_COMM_WORLD);
 if (in[MsgLen/2]*in[MsgLen/2] < -100.0) printf("%f\n",in[MsgLen/2]);
 MPI_Reduce(&t, &tmax, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); 
 if (myid == 0)
 {
   printf("Ring Communication: Message Size = %d; %.2f Gbytes/sec; Time = %.3f sec; \n",
    MsgLen,8*2.0*1e-9*MsgLen*nprocs*Niter/tmax,tmax);
 }
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
