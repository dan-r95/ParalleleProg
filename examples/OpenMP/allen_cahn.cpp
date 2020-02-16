#include <stdio.h>
//#include "stdafx.h"
#include <chrono>
#include <fstream>
#include <iterator>
#include <string>
#include <iostream>
#include <cmath>
#include <random>
#include <cstring>
#include "omp.h"
#include "helpers.h"
using namespace std::chrono;
using namespace std;
#define _PI 3.141592654
int main(int argc, char *argv[])
{
	int N = 1000;
	double epsilon = 2.0;
	double dx = 1.0;
	double dt = 0.02;
	double D = (epsilon * epsilon) / (dx * dx);
	std::mt19937 rng (42);
	std::uniform_real_distribution<double> dis(0.0, 0.2);
	int iterations = 10000;
	int store_every_N_steps = 1000;
	printf("N = %d, k = %d\n", N, iterations);
	// Matrizen anlegen
	Array2D u(N + 1, N + 1);
	Array2D u_old(N + 1, N + 1);
	// Writer
	VTKWriter writer("output_allen_cahn_corrected_");
	/// Anfangswert setzen
	for (int i = 0; i < N + 1; i++)
		for (int j = 0; j < N + 1; j++) {
			u_old(i,j) = u(i,j) = dis(rng);
		}
	// Randbedingung setzen
	for (int i = 0; i < N + 1; i++) {
		u(i,0) = u_old(i,0) = 1.0;
		u(i,N) = u_old(i,N) = 1.0;
		u(0,i) = u_old(0,i) = -1.0;
		u(N,i) = u_old(N,i) = -1.0;
	}
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int k = 0; k <= iterations; k++) // time step iterations
	{
		double sum = 0.0;
#pragma omp parallel for schedule(dynamic) reduction(+:sum)
		for (int i = 1; i < N; i++) {
			for (int j = 1; j < N; j++)
			{
				u(i,j) = u_old(i,j) + dt * (D * (u_old(i-1,j) + u_old(i+1,j) + u_old(i,j-1) + u_old(i,j+1) - 4.0 * u_old(i,j)) + sin(_PI * u_old(i,j)));
				sum += u(i, j);
			}
		}
		double averaged_sum = sum / (N * N);
#pragma omp parallel for schedule(dynamic)
		for (int i = 1; i < N; i++) {
			for (int j = 1; j < N; j++) {
				u(i,j) -= averaged_sum;
			}
		}
		// u und u_old vertauschen nach jedem Zeitschritt
		swap(u, u_old);
		// Ausgabe in jedem 100-ten Zeitschritt
		if (k % store_every_N_steps == 0) {
			// Ausgabe von Zeitschritt und Laufzeit
			printf("k = %i,\t\tCPU time\t= %d ms\n", k, duration_cast<milliseconds>(high_resolution_clock::now() - t1).count());
			//Rausschreiben der Daten in eine Datei
			writer.write_step(u, k);
			writer.finalize();
		}
	}
}
