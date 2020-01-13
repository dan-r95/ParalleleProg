

// #include "stdafx.h"

#include <stdio.h>
#include <chrono>
#include <fstream>
#include <iterator>
#include <string>
#include <iostream>
#include "omp.h"

#include "helpers.h"

using namespace std::chrono;
using namespace std;


int main(int argc, char *argv[])
{
	
	double x_end = 0.2;					// [m]
	double t_end = 100.0;				// [sec]

	double dt = 0.001;					// [sec]
	double delta_x = 0.001;			 	// [m]	
	double D = 98.8e-6;					// [m^2/s]

	double dtDhh = dt * D / (delta_x * delta_x);

	int iterations = (int) t_end / dt;
	int N = (double) x_end / delta_x;

	int store_every_N_steps = 1000;

	printf("N = %d, k = %d\n", N, iterations);

	// Matrizen anlegen
	Array2D u(N + 1, N + 1);
	Array2D u_old(N + 1, N + 1);

	// Writer
	VTKWriter writer("output_heat_");

	/// Anfangswert setzen
	for (int i = 0; i < N + 1; i++)
		for (int j = 0; j < N + 1; j++) {
			u(i, j) = u_old(i, j) = 20.0;
		}

	// Randbedingung setzen
	for (int i = 0; i < N + 1; i++) {
		u(i, 0) = u_old(i, 0) = 90.0;
	}

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	for (int k = 0; k <= iterations; k++) // time step iterations
	{

#pragma omp parallel for schedule(dynamic)
		for (int i = 1; i < N; i++) {
			for (int j = 1; j < N; j++)
			{
				u(i, j) = u_old(i, j) + dtDhh * (u_old(i-1, j) + u_old(i+1, j) + u_old(i, j-1) + u_old(i, j+1) - 4 * u_old(i, j));

				if (i >= N / 4 && i < 3 * N / 4 && j >= N / 4 && j < 3 * N / 4) {
					u(i, j) += dt * 4.0; 
				}
			}
		}

		// u und u_old vertauschen nach jedem Zeitschritt
		swap(u, u_old);

		// Ausgabe in jedem 100-ten Zeitschritt
		if (k % store_every_N_steps == 0) {
			// Ausgabe von Zeitschritt und Laufzeit
			printf("k = %i,\t\tCPU time\t= %d ms\n", 
					k, duration_cast<milliseconds>(high_resolution_clock::now() - t1).count());

			//Rausschreiben der Daten in eine Datei
			writer.write_step(u, k);
			writer.finalize();
		}
	}
}


