
//include "stdafxh"
#include <iostream>
#include <random>
#include <mpi.h>

#include "helpers.h"

int main(int argc, char *argv[])
{

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // now we create the cartesian topology
    int ndims[2] = {0, 0};
    MPI_Dims_create(world_size, 2, ndims);

    int block_dims_i = ndims[0];
    int block_dims_j = ndims[1];

    if (world_rank == 0) {
    	printf("There are %i processes in total which we'll split into %ix%i\n", 
    			world_size, block_dims_i, block_dims_j);
    }

    MPI_Comm comm_cart;
    int periods[2] = {0, 0};
    MPI_Cart_create(MPI_COMM_WORLD, 2, ndims, periods, 1, &comm_cart);

    // get the new rank in the cartesien toplogy
    int cart_rank;
    MPI_Comm_rank(comm_cart, &cart_rank);

    // getting the coordinates of myself and the neighbours
    int left, right, top, bottom;
    int local_coordinates[2];
    MPI_Cart_coords(comm_cart, cart_rank, 2, local_coordinates);

    int block_i = local_coordinates[0];
    int block_j = local_coordinates[1];

    MPI_Cart_shift(comm_cart, 0, 1, &left, &right);
    MPI_Cart_shift(comm_cart, 1, 1, &bottom, &top);	// SWITCHED to map to a more mathematical coordinate system

    printf("I'm %i and my coordinates are (%i, %i), my neighbours: %i, %i, %i, %i (left, right, top, bottom)\n", 
    		cart_rank, block_i, block_j, left, right, top, bottom);

    double dt = 0.02;
	double hx = 1, hy = 1;
	double eps = 2;
	double D = eps*eps;
	double Dhhx = D / hx / hx;
	double Dhhy = D / hy / hy;

	std::mt19937 rng (cart_rank);
	std::uniform_real_distribution<double> dis(-0.1, 0.1);

	int N = 1000;

	// Matrizen anlegen
	Domain2D u(N + 1, N + 1, block_i, block_j);
	Domain2D u_old(N + 1, N + 1, block_i, block_j);

	// Writer
	VTKWriter writer("mpi_allencahn_");

	/// Anfangswert setzen
	for (int i = 0; i < u.get_isize(); i++) {
		for (int j = 0; j < u.get_jsize(); j++) {
			u_old(i, j) = u(i, j) = dis(rng);
		}
	}

	// Randbedingung setzen
	for (int j = 0; j < u.get_jsize(); j++) { 
		u_old(0, j) = u(0, j) = (left == MPI_PROC_NULL) ? 1.0 : u(0, j);
		u_old(N, j) = u(N, j) = (right == MPI_PROC_NULL) ? 1.0 : u(N, j);
	}
	for (int i = 0; i < u.get_isize(); i++) { 
		u_old(i, 0) = u(i, 0) = (bottom == MPI_PROC_NULL) ? -1.0 : u(i, 0);
		u_old(i, N) = u(i, N) = (top == MPI_PROC_NULL) ? -1.0 : u(i, N);
	}


	// creating the buffers to send and recieve
	double send_buff_top[N + 1], send_buff_bottom[N + 1], recv_buff_top[N + 1], recv_buff_bottom[N + 1];
	double send_buff_left[N + 1], send_buff_right[N + 1], recv_buff_left[N + 1], recv_buff_right[N + 1];

	// copying / sending the boundaries (not really needed in this example)
	for (int i = 0; i < u.get_isize(); i++) {
		send_buff_bottom[i] = u_old(i, 1);
		send_buff_top[i] = u_old(i, N - 1);
	}
	for (int j = 0; j < u.get_jsize(); j++) {
		send_buff_left[j] = u_old(1, j);
		send_buff_right[j] = u_old(N - 1, j);
	}

	MPI_Request request[8];

	MPI_Isend(send_buff_left, N + 1, MPI_DOUBLE, left, 17, comm_cart, request);
	MPI_Irecv(recv_buff_left, N + 1, MPI_DOUBLE, left, 17, comm_cart, request + 1);
	MPI_Isend(send_buff_right, N + 1, MPI_DOUBLE, right, 17, comm_cart, request + 2);
	MPI_Irecv(recv_buff_right, N + 1, MPI_DOUBLE, right, 17, comm_cart, request + 3);
	MPI_Isend(send_buff_top, N + 1, MPI_DOUBLE, top, 17, comm_cart, request + 4);
	MPI_Irecv(recv_buff_top, N + 1, MPI_DOUBLE, top, 17, comm_cart, request + 5);
	MPI_Isend(send_buff_bottom, N + 1, MPI_DOUBLE, bottom, 17, comm_cart, request + 6);
	MPI_Irecv(recv_buff_bottom, N + 1, MPI_DOUBLE, bottom, 17, comm_cart, request + 7);

	MPI_Waitall(8, request, MPI_STATUSES_IGNORE);

	for (int j = 0; j < u.get_jsize(); j++) {
		if (left != MPI_PROC_NULL) u_old(0, j) = recv_buff_left[j]; 
		if (right != MPI_PROC_NULL) u_old(N, j) = recv_buff_right[j];
	}
	for (int i = 0; i < u.get_isize(); i++) {
		if (bottom != MPI_PROC_NULL) u_old(i, 0) = recv_buff_bottom[i];
		if (top != MPI_PROC_NULL) u_old(i, N) = recv_buff_top[i];
	}


	double total_calculation_time = 0.0;
	double total_communication_time = 0.0;
	double total_coping_time = 0.0;
	double start_time = MPI_Wtime();

	for (int k = 0; k < 1000; k++) // time step iterations
	{
		double du_max = 0.0;

		double start_calculation = MPI_Wtime();

		for (int i = 1; i < u.get_isize() - 1; i++) {
			for (int j = 1; j < u.get_jsize() - 1; j++) {
				double du = Dhhx*(u_old(i - 1, j) + u_old(i + 1, j) - 2.0*u_old(i, j)) +
							Dhhy*(u_old(i, j - 1) + u_old(i, j + 1) - 2.0*u_old(i, j)) + 
							sin(3.141592654*(u_old(i, j)));
				u(i, j) = u_old(i, j) + dt * du;
				if (du_max < du) du_max = du;

			}
		}

		double end_calculation = MPI_Wtime();

		std::swap(u, u_old);

		double start_copying = MPI_Wtime();

		for (int j = 0; j < u.get_jsize(); j++) {
			send_buff_left[j] = u(1, j);
			send_buff_right[j] = u(N - 1, j);
		}
		for (int i = 0; i < u.get_isize(); i++) {
			send_buff_bottom[i] = u(i, 1);
			send_buff_top[i] = u(i, N - 1);
		}

		double end_copying = MPI_Wtime();


		double start_communication = MPI_Wtime();

		MPI_Request request[8];

		MPI_Isend(send_buff_left, N + 1, MPI_DOUBLE, left, 17, comm_cart, request);
		MPI_Irecv(recv_buff_left, N + 1, MPI_DOUBLE, left, 17, comm_cart, request + 1);
		MPI_Isend(send_buff_right, N + 1, MPI_DOUBLE, right, 17, comm_cart, request + 2);
		MPI_Irecv(recv_buff_right, N + 1, MPI_DOUBLE, right, 17, comm_cart, request + 3);
		MPI_Isend(send_buff_top, N + 1, MPI_DOUBLE, top, 17, comm_cart, request + 4);
		MPI_Irecv(recv_buff_top, N + 1, MPI_DOUBLE, top, 17, comm_cart, request + 5);
		MPI_Isend(send_buff_bottom, N + 1, MPI_DOUBLE, bottom, 17, comm_cart, request + 6);
		MPI_Irecv(recv_buff_bottom, N + 1, MPI_DOUBLE, bottom, 17, comm_cart, request + 7);

		MPI_Waitall(8, request, MPI_STATUSES_IGNORE);

		double end_communication = MPI_Wtime();

		double start_setting = MPI_Wtime();

		for (int j = 0; j < u.get_jsize(); j++) {
			if (left != MPI_PROC_NULL) u_old(0, j) = recv_buff_left[j];
			if (right != MPI_PROC_NULL) u_old(N, j) = recv_buff_right[j];
		}
		for (int i = 0; i < u.get_isize(); i++) {
			if (bottom != MPI_PROC_NULL) u_old(i, 0) = recv_buff_bottom[i];
			if (top != MPI_PROC_NULL) u_old(i, N) = recv_buff_top[i];
		}

		double end_setting = MPI_Wtime();


		total_calculation_time += end_calculation - start_calculation;
		total_coping_time += end_copying - start_copying + end_setting - start_setting;
		total_communication_time += end_communication - start_communication;


		// Ausgabe in jedem 100-ten Zeitschritt
		if (k % 100 == 0) {

			double du_max_global;
			MPI_Allreduce(&du_max, &du_max_global, 1, MPI_DOUBLE, MPI_MAX, comm_cart);
			if (cart_rank == 0) printf("k = %i, global max in change: %1.6f \n", k, du_max_global);

			//Rausschreiben der Daten in eine Datei
			writer.write_step(u, k);
			if (cart_rank == 0) {
				writer.finalize_step(k, N+1, N+1, block_dims_i, block_dims_j);
				writer.finalize();		
			}
		}
	}


	if (cart_rank == 0) printf("Timings: total %e, computation %e,  communication %e, copying %e \n", 
								MPI_Wtime() - start_time, 
								total_calculation_time,
								total_communication_time,
								total_coping_time);

	MPI_Finalize();

}

