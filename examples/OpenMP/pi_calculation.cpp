
// Example solution for exercise 2 (pi calculation)

/*	Compiling on Unix
g++ -std=c++11 -O3 -fopenmp pi_calculation.cpp -o pi_calculation
export OMP_NUM_THREADS=4
./pi_calculation
*/

//#include "stdafx.h"

#include <cmath>
#include <iostream>
#include <chrono>
#include <vector>

#include <omp.h>

#define _PI 3.14159265358979323846264
#define _N 1e9


// Sequential pi calculation
double calculate_pi(long N) {

	double pi = 0.0;
	double N_inverse = 1.0 / (double)N;

	for (long i = 0; i < N; ++i) {
		double x_mid = ((double)i + 0.5) * N_inverse;
		pi += 4.0 / (1.0 + x_mid * x_mid);
	}

	return pi * N_inverse;
}

// First version with a critical section => slow!
double calculate_pi_critical1(long N) {

	double pi = 0.0;
	double N_inverse = 1.0 / (double)N;


#pragma omp parallel for 
	for (long i = 0; i < N; ++i) {
		double x_mid = ((double)i + 0.5) * N_inverse;
#pragma omp critical
		pi += 4.0 / (1.0 + x_mid * x_mid);
	}

	return pi * N_inverse;
}


// Second critical version with local variable => much faster
double calculate_pi_critical2(long N) {

	double pi = 0.0;
	double local_sum;
	double N_inverse = 1.0 / (double) N;

#pragma omp parallel private (local_sum)
	{
		local_sum = 0.0;
#pragma omp for
		for (long i = 0; i < N; ++i) {
			double x_mid = ((double)i + 0.5) * N_inverse;
			local_sum += 4.0 / (1.0 + x_mid * x_mid);
		}

#pragma omp critical
		pi = pi + local_sum;
	}

	return pi * N_inverse;
}

// Strided access example, leading to false sharing for small strides
double calculate_pi_strided(long N, int stride, int num_threads) {

	std::vector<double> pis(num_threads * stride, 0.0);

	double N_inverse = 1.0 / (double)N;

#pragma omp parallel
	{
		int id = omp_get_thread_num();
#pragma omp for
		for (long i = 0; i < N; ++i) {
			double x_mid = ((double)i + 0.5) * N_inverse;
			pis[id * stride] += 4.0 / (1.0 + x_mid * x_mid);
		}
	}

	double pi = 0.0;
	for (int i = 0; i < num_threads * stride; i+=stride) {
		pi += pis[i];
	}
	return pi * N_inverse;
}

// Reduction version
double calculate_pi_reduction(long N) {

	double pi = 0.0;
	double N_inverse = 1.0 / (double) N;

#pragma omp parallel for reduction(+:pi)
	for (long i = 0; i < N; ++i) {
		double x_mid = ((double) i + 0.5) * N_inverse;
		pi += 4.0 / (1.0 + x_mid * x_mid);
	}

	return pi * N_inverse;
}

int main()
{

	int num_threads = 0;

#ifdef _OPENMP
	std::printf("This Code was compiled with OpenMP\n");
#pragma omp parallel
#pragma omp single
	{
	num_threads = omp_get_num_threads();
	std::printf("Number of Threads: %d\n", num_threads);
	}

#else
	std::printf("This code is not compiled with OpenMP!\n");
#endif // _OPENMP


	double pi = 0.0;

	auto start = std::chrono::steady_clock::now();
	pi = calculate_pi(_N);
	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;
	double time_serial = std::chrono::duration <double, std::milli>(diff).count();
	double error = std::abs(_PI - pi);
	printf("Sequential: \t\t time: %11.5fms (abs. error: %.1e)\n", std::chrono::duration <double, std::milli>(diff).count(), error);


	start = std::chrono::steady_clock::now();
	pi = calculate_pi_critical1(_N);
	end = std::chrono::steady_clock::now();

	diff = end - start;
	double time = std::chrono::duration <double, std::milli>(diff).count();
	error = std::abs(_PI - pi);
	printf("Critical (suboptimal): \t time: %11.5fms, speedup: %f (abs. error: %.1e)\n", time, time_serial / time, error);

	start = std::chrono::steady_clock::now();
	pi = calculate_pi_critical2(_N);
	end = std::chrono::steady_clock::now();

	diff = end - start;
	time = std::chrono::duration <double, std::milli>(diff).count();
	error = std::abs(_PI - pi);
	printf("Critical (better): \t time: %11.5fms, speedup: %f (abs. error: %.1e)\n", time, time_serial / time, error);


	for (int i = 1; i <= 128; i*=2) {

		start = std::chrono::steady_clock::now();
		pi = calculate_pi_strided(_N, i, num_threads);
		end = std::chrono::steady_clock::now();
		
		diff = end - start;
		time = std::chrono::duration <double, std::milli>(diff).count();
		error = std::abs(_PI - pi);
		printf("Strided (%d): \t\t time: %11.5fms, speedup: %f (abs. error: %.1e)\n", i, time, time_serial / time, error);
	}

	start = std::chrono::steady_clock::now();
	pi = calculate_pi_reduction(_N);
	end = std::chrono::steady_clock::now();

	diff = end - start;
	time = std::chrono::duration <double, std::milli>(diff).count();
	error = std::abs(_PI - pi);
	printf("Reduction: \t\t time: %11.5fms, speedup: %f (abs. error: %.1e)\n", time, time_serial / time, error);

    return 0;
}

