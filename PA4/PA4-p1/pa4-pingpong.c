#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[]) {

	double rtclock();
	int myid, nprocs, namelength;
	char processorname[1024];


	double clkbegin, clkend;
	double t;
	int i,j,it;
	int MsgLen,Niter;
	MPI_Status status;
	double *A,*B;

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &myid );
	MPI_Comm_size( MPI_COMM_WORLD, &nprocs );
	MPI_Get_processor_name(processorname, &namelength);

	printf("Process id=%d; Processor id =%s\n",myid,processorname);

  MPI_Barrier(MPI_COMM_WORLD);

	if (myid == 0) 
	{
		printf("Enter Message Length ");
		scanf("%d",&MsgLen);
		printf("Number of spins around ring ");
		scanf("%d",&Niter);
	}
	MPI_Bcast(&MsgLen, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&Niter, 1, MPI_INT, 0, MPI_COMM_WORLD);
	A = (double *) malloc(MsgLen*sizeof(double));
	B = (double *) malloc(MsgLen*sizeof(double));

	MPI_Barrier(MPI_COMM_WORLD);
	// Start timer
  clkbegin = rtclock();

  if (myid == 0) {
    for (i = 0; i < Niter; i++) {
     MPI_Send(A, MsgLen, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
     MPI_Recv(B, MsgLen, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
     MPI_Send(B, MsgLen, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
     MPI_Recv(A, MsgLen, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
   }
 } else if (myid == 1) {
  for (i = 0; i < Niter; i++) {
   MPI_Recv(A, MsgLen, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
   MPI_Send(A, MsgLen, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
   MPI_Recv(B, MsgLen, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
   MPI_Send(B, MsgLen, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
 }
}

	// Stop timer
clkend = rtclock();

t = clkend - clkbegin;
if (myid == 0)
{
  printf("Communication: Message Size = %d; %.2f Gbytes/sec; Time = %.3f sec; \n",
    MsgLen,8*4.0*1e-9*MsgLen*Niter/t,t);
}

MPI_Finalize();
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
