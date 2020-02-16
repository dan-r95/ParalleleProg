#include <mpi.h>
#include <stdio.h>
#include <string>
void virtual_ring_communication() {
	// get the rank and size
	int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // creating the virtual ring topology
    MPI_Comm ring_communicator;
    int dim_size[1] = {size};
    int periods[1] = {1};
    MPI_Cart_create(MPI_COMM_WORLD, 1, dim_size, periods, 1, &ring_communicator);
    int ring_size, ring_rank;
    MPI_Comm_size(ring_communicator, &ring_size);
    MPI_Comm_rank(ring_communicator, &ring_rank);
    // finding neighbours
    int left, right;
    MPI_Cart_shift(ring_communicator, 0, 1, &left, &right);
    int sum = 0;
    int recv_buffer;
    int send_buffer = rank;
    MPI_Request send_request;
    for (int i = 0; i < size; i++) {
    	MPI_Issend(&send_buffer, 1, MPI_INT, right, 42, MPI_COMM_WORLD, &send_request);
    	MPI_Recv(&recv_buffer, 1, MPI_INT, left, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    	// now we wait until the MPI_Issend is completed and we can modify the send_buffer
    	MPI_Wait(&send_request, MPI_STATUS_IGNORE);
    	// now we can overwrite the send buffer
    	send_buffer = recv_buffer;
    	sum += recv_buffer;
    }
    printf("I'm process %i and calculated a sum of %i\n", rank, sum);
}
int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    virtual_ring_communication();
    // Finalize the MPI environment.
    MPI_Finalize();
}