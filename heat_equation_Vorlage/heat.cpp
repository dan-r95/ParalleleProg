#include <stdio.h>
#include "stdafx.h"
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
	int N = 200;

	// Matrizen anlegen
	Array2D u(N + 1, N + 1);
	Array2D u_old(N + 1, N + 1);

	Array2D q(N + 1, N + 1);

	// Writer
	VTKWriter writer("output_");

	/// Anfangswert setzen
	for (int i = 0; i < N + 1; i++)
		for (int j = 0; j < N + 1; j++)
		{
			// e.g. u(i, j) = 20;
			u(i, j) = u_old(i, j) = 20;
		}

	// Randbedingung setzen
	// linker rand auf 90 grad erw�rmt
	for (int i = 0; i < N + 1; i++)
	{
		u(0, i) = u_old(0, i) = 90; // linker rand mit 90 grad initialisieren
	}

	/*
	Der Autohersteller f�gt eine W�rmequelle in der Mitte der Platte hinzu.
		Die St�rke der W�rmequelle ist q = 4� / sec, die W�rmequelle wirkt nur in
		einem 10cm x 10cm gro�en St�ck in der Mitte der Platte.Wie sieht die
		W�rmeverteilung jetzt aus ?*/
	for (int i = 0; i < N + 1; i++)
	{									// wegen randbedingungen, die nicht mit einbezogen werden sollen   x -achse
		for (int j = 0; j < N + 1; j++) // y- achse
		{
			q(i, j) = 0;
			if (N * 0.25 < i < N * 0.75 && N * 0.25 < j < N * 0.75)
			{
				q(i, j) = 4;
			}
		}
	}

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	double leitfaehigkeit = 98.8e-6; // w�rmeleitf�higkeit
	double deltaX = 0.001;			 //0.001 m
	double deltaY = 0.001;			 //0.001 m
	double deltaT = 0.001;			 //0.001 sec

	for (int k = 0; k < 100000; k++) // time step iterations  100000
	{
		for (int i = 1; i < N; i++)		// wegen randbedingungen, die nicht mit einbezogen werden sollen   x -achse
			for (int j = 1; j < N; j++) // y- achse
			{
				/// Iterationsvorschrift zur Berechnung von u zur neuen Zeit
				// delta x2 = 1/ N ^2

				if (N * 0.25 < i < N * 0.75 && N * 0.25 < j < N * 0.75)
				{
					// konstante w�rme in der mitte
					u(i, j) = ((leitfaehigkeit * (u_old(i + 1, j) - 2 * u_old(i, j) + u_old(i - 1, j) + u_old(i, j + 1) - 2 * u_old(i, j) + u_old(i, j - 1))) / (deltaX * deltaX)) * deltaT + u_old(i, j) + q(i, j) * deltaT;
				}
				else
				{
					u(i, j) = ((leitfaehigkeit * (u_old(i + 1, j) - 2 * u_old(i, j) + u_old(i - 1, j) + u_old(i, j + 1) - 2 * u_old(i, j) + u_old(i, j - 1))) / (deltaX * deltaX)) * deltaT + u_old(i, j);
				}
			}

		// u und u_old vertauschen nach jedem Zeitschritt
		swap(u, u_old);

		// Ausgabe in jedem 100-ten Zeitschritt
		if (k % 100 == 0)
		{
			// Ausgabe von Zeitschritt und Laufzeit
			printf("k = %i,  CPU time (clock)                = %d ms\n", k, duration_cast<milliseconds>(high_resolution_clock::now() - t1).count());

			// Ausgabe der Temperatur in der Mitte der Platte
			printf("Temperatur in der Mitte: %1.1f ", u(N / 2, N / 2));

			// Ausgabe der L�sung auf dem Bildschirm (nur f�r N<20 sinnvoll)
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
}
