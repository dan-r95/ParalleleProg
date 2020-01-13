
#include "stdafx.h"
#include <iostream>
#include <random>
#include <mpi.h>
#include <math.h>

#include "helpers.h"

int main(int argc, char* argv[])
{
	// Initialize the MPI environment
	MPI_Init(&argc, &argv);

	int block_dims_i = 4;    // TODO: set the dimensions of the MPI topologie, e.g. 2x2
	int block_dims_j = 4;

	// TODO: Set up virtual topologie...
	int ndims = 2;
	int dims[] = { block_dims_i, block_dims_j };
	int periods[] = { 0, 0 };
	MPI_Comm comm_cart;
	MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, 0, &comm_cart);

	// create and get world_wank
	int world_rank;
	MPI_Comm_rank(comm_cart, &world_rank);

	int my_coords[2] = { };
	MPI_Cart_coords(comm_cart, world_rank, ndims, my_coords);

	int block_i = my_coords[0];    // TODO: set the right block IDs (MPI cartesian coordinates)
	int block_j = my_coords[1];
	int cart_rank = world_rank;


	int neighbour_N, neighbour_S, neighbour_W, neighbour_O;
	MPI_Cart_shift(comm_cart, 0, 1,    // direction 0 -> X
		&neighbour_W, &neighbour_O);
	MPI_Cart_shift(comm_cart, 1, 1,    // direction 0 -> Y		//swap bottom with top 
		&neighbour_N, &neighbour_S);

	printf("I'm %i and my coordinates are (%i, %i), my neighbours: %i, %i, %i, %i (left, right, top, bottom)\n",
		cart_rank, block_i, block_j, neighbour_W, neighbour_O, neighbour_N, neighbour_S);

	// Simulation parameters
	const int N = 50;

	// random number generator, seed based on Cartesian rank of MPI process
	std::mt19937 rng(cart_rank);
	std::uniform_real_distribution<double> dis(0, 0.2);   // neue startbedingungen mit abziehen des phasen durchschnitts 

	// Defining the domains (which know the MPI coordinates)
	Domain2D u(N + 1, N + 1, block_i, block_j);
	Domain2D u_old(N + 1, N + 1, block_i, block_j);

	// Writer
	VTKWriter writer("mpi_allencahn_");

	/// Initial values
	for (int i = 0; i < u.get_isize(); i++) {
		for (int j = 0; j < u.get_jsize(); j++) {
			u_old(i, j) = u(i, j) = dis(rng);// set random values
		}
	}

	// TODO set boundary conditions based on coordinates
	// Boundary Conditions
	// ränder - 0 oder 1
	for (int j = 0; j < u.get_jsize(); j++) {
		//east and west equals 1
		if (neighbour_W == MPI_PROC_NULL) {    // links
			u_old(0, j) = u(0, j) = 1;
		}
		if (neighbour_O == MPI_PROC_NULL) {		// rechts
			u_old(N, j) = u(N, j) = 1;
		}
	}
	for (int i = 0; i < u.get_isize(); i++) {
		// south and north equals -1 
		if (neighbour_S == MPI_PROC_NULL) {   // unten  
			u_old(i, N) = u(i, N) = -1;
		}
		if (neighbour_N == MPI_PROC_NULL) {   // oben  
			u_old(i, 0) = u(i, 0) = -1;
		}
	}

	// define halo ring arrays, this means 4 arrays for 4 directions - receiving and sending

	double send_N[N + 1] = {}, send_W[N + 1] = {}, send_O[N + 1] = {}, send_S[N + 1] = {}; // init with zero
	double recv_N[N + 1] = {}, recv_W[N + 1] = {}, recv_O[N + 1] = {}, recv_S[N + 1] = {}; // init with zero

	// MPI proc null wenn kein nachbar

	double start_time = MPI_Wtime();

	for (int k = 0; k < 3000; k++) // time step iterations
	{
		double sendSum;
		for (int i = 1; i < u.get_isize() - 1; i++) {
			for (int j = 1; j < u.get_jsize() - 1; j++) {

				//Make a time step by updating u from u_old
				u(i, j) = (u_old(i + 1, j) + u_old(i - 1, j) + u_old(i, j + 1) + u_old(i, j - 1) - 4 * u_old(i, j)) * 0.08 + 0.02 * sin(u_old(i, j) * 3.14) + u_old(i, j);
				sendSum += u(i, j);
				/**
				um sicherzustellen, dass
				beide Phasen immer genau zu gleichen Anteilen vorhanden sind, soll ein
				Korrekturverfahren implementiert werden
				**/

			}
		}
		double recvSum;
		MPI_Allreduce(&sendSum, &recvSum, 1, MPI_DOUBLE, MPI_SUM, comm_cart);
		double average = (double)recvSum / (double)(N * N * block_dims_i * block_dims_j);
		for (int i = 1; i < u.get_isize() - 1; i++) {
			for (int j = 1; j < u.get_jsize() - 1; j++) {
				// subtract the avg
				u(i, j) = u(i, j) - average;
			}
		}

		sendSum = 0;


		// fill halos for sending
		for (int j = 0; j < u.get_jsize(); j++) {

			send_W[j] = u(1, j);
			send_N[j] = u(j, 1);
			send_O[j] = u(N - 1, j);
			send_S[j] = u(j, N - 1);
		}

		std::swap(u, u_old);

		// TODO: Exchange the halos
		if (neighbour_W != MPI_PROC_NULL) {    // links
			MPI_Send(&send_W, N, MPI_DOUBLE,
				neighbour_W, 1, comm_cart);
			MPI_Recv(&recv_W, N, MPI_DOUBLE,
				neighbour_W, 1, comm_cart, MPI_STATUSES_IGNORE);
		}
		if (neighbour_O != MPI_PROC_NULL) {		// rechts
			MPI_Send(&send_O, N, MPI_DOUBLE,
				neighbour_O, 1, comm_cart);
			MPI_Recv(&recv_O, N, MPI_DOUBLE,
				neighbour_O, 1, comm_cart, MPI_STATUSES_IGNORE);
		}

		if (neighbour_S != MPI_PROC_NULL) {   // unten  
			MPI_Send(&send_S, N, MPI_DOUBLE,
				neighbour_S, 1, comm_cart);
			MPI_Recv(&recv_S, N, MPI_DOUBLE,
				neighbour_S, 1, comm_cart, MPI_STATUSES_IGNORE);
		}
		if (neighbour_N != MPI_PROC_NULL) {   // oben  
			MPI_Send(&send_N, N, MPI_DOUBLE,
				neighbour_N, 1, comm_cart);
			MPI_Recv(&recv_N, N, MPI_DOUBLE,
				neighbour_N, 1, comm_cart, MPI_STATUSES_IGNORE);
		}

		// fill u with received halos when there is a neighbour! otherwise not
		for (int j = 0; j < u.get_jsize(); j++) {
			if (neighbour_N != MPI_PROC_NULL) {   // oben  
				u_old(j, 0) = recv_N[j];
			}
			if (neighbour_W != MPI_PROC_NULL) {
				u_old(0, j) = recv_W[j];
			}
			if (neighbour_O != MPI_PROC_NULL) {
				u_old(N, j) = recv_O[j];
			}
			if (neighbour_S != MPI_PROC_NULL) {
				u_old(j, N) = recv_S[j];
			}
		}



		// Writing result
		if (k % 100 == 0) {

			writer.write_step(u, k);

			// One process needs to write the pvts and the pvd file
			if (cart_rank == 0) {
				writer.finalize_step(k, N + 1, N + 1, block_dims_i, block_dims_j);
				writer.finalize();
			}
		}
	}

	if (cart_rank == 0) printf("Total Time passed %e \n", MPI_Wtime() - start_time);

	MPI_Finalize();

}

