#include <mpi.h>
#include <stdio.h>
#include <string>
#define _N 100
#define _MAX_ELEMENTS 262144 // 2MB / 8
void simple_ping_ping() {
	 // Get the number of processes
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char recv_buffer[_MAX_ELEMENTS];	// big enough receive buffer
    double start = MPI_Wtime();
    for (int i = 0; i < _N; i++) {
    	if (rank == 0) {	// we are root, start sending
    		std::string message("Ping");
    		MPI_Send(message.c_str(), message.length(), MPI_CHAR, 1, 42, MPI_COMM_WORLD);
    		// recieving the answer
    		MPI_Status status; 
    		MPI_Recv(&recv_buffer, _MAX_ELEMENTS, MPI_CHAR, 1, 43, MPI_COMM_WORLD, &status);
    		int recv_elements;
    		MPI_Get_count(&status, MPI_CHAR, &recv_elements);
    		std::string answer(recv_buffer, recv_elements);
    		printf("I'm Process 0 and received a %s\n", answer.c_str());
    	} else {			// we are the slave, start receving
    		MPI_Status status; 
    		MPI_Recv(&recv_buffer, _MAX_ELEMENTS, MPI_CHAR, 0, 42, MPI_COMM_WORLD, &status);
    		int recv_elements;
    		MPI_Get_count(&status, MPI_CHAR, &recv_elements);
    		std::string answer(recv_buffer, recv_elements);
    		printf("I'm Process 1 and received a %s\n", answer.c_str());
  			// sending the Pong
    		std::string message("Pong");
    		MPI_Send(message.c_str(), message.length(), MPI_CHAR, 0, 43, MPI_COMM_WORLD);
    	}
    }
    double end = MPI_Wtime();
    if (rank == 0 ) {
    	printf("Execution time of one ping: %f microseconds\n", (end-start) / (2 * _N) * 1e6);
	}	
}
void advanced_ping_pong() {
		 // Get the number of processes
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int message = 42;
    // sending a first ping pong so the measurements does not include process warm up
	if (rank == 0) {	// we are root, start sending
		MPI_Send(&message, 1, MPI_INT, 1, 42, MPI_COMM_WORLD);
		MPI_Status status; 
		MPI_Recv(&message, 1, MPI_INT, 1, 43, MPI_COMM_WORLD, &status);
	} else {			// we are the slave, start receving
		MPI_Status status; 
		MPI_Recv(&message, 1, MPI_INT, 0, 42, MPI_COMM_WORLD, &status);
		MPI_Send(&message, 1, MPI_INT, 0, 43, MPI_COMM_WORLD);
	}	
	double send_buffer[_MAX_ELEMENTS];	
    double recv_buffer[_MAX_ELEMENTS];	
    for (int message_size = 1; message_size <= 262144; message_size *= 64) {
    	double start = MPI_Wtime();
	    for (int i = 0; i < _N; i++) {
	    	if (rank == 0) {	// we are root, start sending
	    		MPI_Send(&send_buffer, message_size, MPI_DOUBLE, 1, 42, MPI_COMM_WORLD);
	    		MPI_Status status; 
	    		MPI_Recv(&recv_buffer, message_size, MPI_DOUBLE, 1, 43, MPI_COMM_WORLD, &status);
	    	} else {			// we are the slave, start receving
	    		MPI_Status status; 
	    		MPI_Recv(&recv_buffer, message_size, MPI_DOUBLE, 0, 42, MPI_COMM_WORLD, &status);
	    		MPI_Send(&send_buffer, message_size, MPI_DOUBLE, 0, 43, MPI_COMM_WORLD);
	    	}
	    }
	    double end = MPI_Wtime();
    	if (rank == 0 ) {
    		printf("Execution time (size 8x %i): %.2f microseconds => bandwidth of %.2f GB/s\n", message_size, (end-start) / (2 * _N) * 1e6, message_size * 8.0 / ((end-start) / (2 * _N)) / 1e9);
		}	
	}
}
int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    simple_ping_ping();
    advanced_ping_pong();
    // Finalize the MPI environment.
    MPI_Finalize();
}