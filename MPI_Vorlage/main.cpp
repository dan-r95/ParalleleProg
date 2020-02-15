
//#include "stdafx.h"
#include <iostream>
#include <random>
#include <mpi.h>

using namespace std;


int fuerdich(int argc, char* argv[])
{

	// Initialize the MPI environment

	MPI_Init(&argc, &argv);

	int rank;
	int size;
	int sum = 0;



	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	//printf("Hallo %i von %i", rank, size);

	int s_buf = 0;
	int r_buf = 0;
	int resultbuf = 0;
	//printf("Hello World");

	//MPI_Reduce(&rank, &resultbuf, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	//printf("Zahl: \i\n",resu017ltbuf);

	MPI_Comm NEW_WORLD;
	int dims[1] = { 3 };
	int period[1] = { 1 };
	MPI_Cart_create(MPI_COMM_WORLD, 1, dims, period, 1, &NEW_WORLD);


	s_buf = rank;

	//ndim 1
	//dims 3
	//period 1
	//reorder 1

	for (size_t i = 0; i < 3; i++)
	{



		int dest;// = (rank + 1) % size;
		int src;// = (rank - 1 + size) % size;

		MPI_Cart_shift(NEW_WORLD, 0, 1, &src, &dest);


		MPI_Request request_handle;

		MPI_Isend(&s_buf, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, &request_handle);

		MPI_Recv(&r_buf, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

		sum += r_buf;
		s_buf = r_buf;





		MPI_Wait(&request_handle, MPI_STATUSES_IGNORE);


	}
	printf("Nr. %i hat die Zahl %i erhalten! Summe nun: %i | resultbuf = \i\n", rank, r_buf, sum, resultbuf);



	if (rank == 0) {
		//char HelloStr[] = "Hello World";
		//
		//printf("Hallo");

	}
	else if (rank == 1) {
		//	char helleoStr[12];
		//	MPI_Recv(helleoStr,_countof(helleoStr),MPI_CHAR,0,0,MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		//	printf("Rank 1 %s from Rank 0 [n", helleoStr);

	}


	MPI_Finalize();

	return 0;

	// Finalize MPI
}

void createUsingCartestionCoordinates()
{
	// create 3 sub programms all executing this
	// Initialize the MPI environment
	MPI_Comm comm_old;
	int dims = 1;
	int reorder = 1;
	MPI_Comm comm_cart;
	int periods = (1, 0);
	int dim_arr = (3, 1);
	MPI_Cart_create(MPI_COMM_WORLD, dims, &dim_arr, &periods, reorder, &comm_cart);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	cout << world_rank << endl;


	int sumOfMyWorldRanks;
	int sum;
	//global reduction    to get the world rank sum  --> 3
	//MPI_Allreduce(&world_rank, &sumOfMyWorldRanks, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	for (int t = 0; t < 3; t++) {
		int* buf;
		int* sendBuf = &world_rank;

	}


	if (world_rank == 2) {   // just output the first, since this is executed on all cpus
		//cout << "My global sum: " << sumOfMyWorldRanks << endl;
		//cout << "Global sum via recv send loop " << sum << endl;
	}

	MPI_Finalize();

	// end of collective communication
}


int main(int argc, char** argv[])
{

	fuerdich(argc, *argv);
	// create 3 sub programms all executing this
	// Initialize the MPI environment
	MPI_Init(&argc, argv);

	// Finalize MPI
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	char processor_name[MPI_MAX_PROCESSOR_NAME];

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	//cout << world_rank << endl;


	int sumOfMyWorldRanks = 0;
	int sum = 0;
	//global reduction    to get the world rank sum  --> 3
	//MPI_Allreduce(&world_rank, &sumOfMyWorldRanks, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	for (int t = 0; t < 3; t++) {
		int buf;
		cout << "before send buf writing";
		int sendBuf = world_rank;

		int dest = (world_rank + 1) % world_size;
		int source = (world_rank + world_size) % world_size;
		cout << "done init dest & source";
		MPI_Request requestHandler;

		cout << "vor dem senden";
		MPI_Isend(&sendBuf, 1, MPI_INT, dest, 1, MPI_COMM_WORLD, &requestHandler);
		cout << "ich habe gesendet";
		MPI_Recv(&buf, 1, MPI_INT, source, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

		sum += buf;
		MPI_Wait(&requestHandler, MPI_STATUSES_IGNORE);
		sendBuf = buf;
	}


	if (world_rank == 2) {   // just output the first, since this is executed on all cpus
	//	cout << "My global sum: " << sumOfMyWorldRanks << endl;
		cout << "Global sum via recv send loop " << sum << endl;
	}


	// aufgabe slide 156
	//createUsingCartestionCoordinates();


	MPI_Finalize();

	// end of collective communication
}




