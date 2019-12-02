#include <stdio.h>
#include "stdafx.h"
#include <chrono>
#include <fstream>
#include <iterator>
#include <string>
#include <iostream>
#include "omp.h"

#include "helpers.h"
#include <random>
#include<math.h>

using namespace std::chrono;
using namespace std;


int main(int argc, char* argv[])
{
	int N = 1000;
	int timesteps = 10000;  //10k == 10sec
	double pi = 3.14;

	// Matrizen anlegen
	Array2D u(N + 1, N + 1);
	Array2D u_old(N + 1, N + 1);

	Array2D q(N + 1, N + 1);

	// Writer
	VTKWriter writer("output_");

	/// Anfangswert setzen
	//Anfangsbedingung: 𝜙 ∈ [−0.1, +0.1] zufällig,	gleichmäßig	verteilt

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(-0.1, 0.1);

	for (int i = 0; i < N + 1; i++) {
		for (int j = 0; j < N + 1; j++) {
			// e.g. u(i, j) = 20;
			u(i, j) = u_old(i, j) = dis(gen);
		}
	}

	// Randbedingung setzen
	for (int i = 0; i < N + 1; i++) {
		u(0, i) = u_old(0, i) = 1;   // linker rand mit 1 initialisieren
		u(N, i) = u_old(N, i) = 1;   // rechter rand mit 1 initialisieren
		u(i, 0) = u_old(i, 0) = -1;   // oberer rand mit -1 initialisieren
		u(i, N) = u_old(i, N) = -1;   // unterer rand mit -1 initialisieren

	}




	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	double epsilon_quadrat = 4; //2^2
	double deltaX = 1;  //0.001 m
	double deltaY = 1;  //0.001 m
	double deltaT = 0.02;  //0.001 sec

	// zeititeration können wir nicht parallelisieren, weil abhängig vom vorherigen zeitschritt
	for (int k = 0; k < timesteps; k++) // time step iterations  100000
	{
		// einfacher die äußere schleife parallelsieren
//#pragma omp parallel for collapse(2)
#pragma omp parallel for schedule(static)  //auto
		for (int i = 1; i < N; i++)   // wegen randbedingungen, die nicht mit einbezogen werden sollen   x -achse
			for (int j = 1; j < N; j++)  // y- achse
			{
				/// Iterationsvorschrift zur Berechnung von u zur neuen Zeit
				//𝜕^ 𝜙 = 𝜖`a𝜕bb𝜙 + 𝜕cc𝜙d + sin(𝜋𝜙), für	alle	x, y∈(0, N) und
				//	alle	t > 0



				//u(i, j) = ((epsilon_quadrat * (u_old(i + 1, j) - 2 * u_old(i, j) + u_old(i - 1, j) + u_old(i, j + 1) - 2 * u_old(i, j) + u_old(i, j - 1))) / (deltaX * deltaX)) * deltaT + u_old(i, j) + sin(u(i, j) * pi * deltaT);
				u(i, j) = (u_old(i + 1, j) + u_old(i - 1, j) + u_old(i, j + 1) + u_old(i, j - 1) - 4 * u_old(i, j)) * 0.08 + 0.02 * sin(u_old(i, j) * pi) + u_old(i, j);
			}

		// u und u_old vertauschen nach jedem Zeitschritt
		swap(u, u_old);

		// Ausgabe in jedem 100-ten Zeitschritt
		if (k % 100 == 0) {
			// Ausgabe von Zeitschritt und Laufzeit


			// Ausgabe der Temperatur in der Mitte der Platte
			printf("Temperatur in der Mitte: %1.1f ", u(N / 2, N / 2));
			printf("k= %d  CPU time (clock)                = %d ms\n", k, duration_cast<milliseconds>(high_resolution_clock::now() - t1).count());

			// Ausgabe der Lösung auf dem Bildschirm (nur für N<20 sinnvoll)
			/*for (int i = 0; i < N + 1; i++) {
			for (int j = 0; j < N + 1; j++) {
			printf("%1.1f ", u_old(i, j));
			}
			printf("\n");
			} /**/

			//Rausschreiben der Daten in eine Datei
			writer.write_step(u, k);
			writer.finalize();
		}
	}
	printf("  CPU time (clock)                = %d ms\n", duration_cast<milliseconds>(high_resolution_clock::now() - t1).count());
}


