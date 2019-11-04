using System;
using System.Diagnostics;
using System.Numerics;
using System.Windows;

namespace ParalleleProgrammierungPrakt
{
    class Prakt1
    {


        static void aufgabe1()
        {  // Aufgabe 1
            DateTime dt = DateTime.Now;
            Console.WriteLine(dt.ToString());
            Console.WriteLine("Hello World!");

            int size = 1000000;
            int[] a = createArrayOfSize(size);
            int[] b = createArrayOfSize(size);

            int[] result = new int[size];

            int endResult = 0;
            for (int i = 0; i < size; i++)
            {
                endResult += (a[i] * b[i]);
            }

            Console.WriteLine(endResult);

            DateTime end = DateTime.Now;
            Console.WriteLine("Elapsed: " + (end - dt).TotalSeconds + (" s"));

        }

        public static void aufgabe2()
        {  // Aufgabe 1
            DateTime dt = DateTime.Now;
            Console.WriteLine(dt.ToString());
            Console.WriteLine("Hello World!");

            int SPALTEN = 200;
            int REIHEN = 400;
            int[,] matrix = new int[REIHEN, SPALTEN];  //zeile, spalte
            int[] vektor = new int[REIHEN];  // spaltenanzahl matrix = anzahl vektorelemente
            int skalarprodukt = 0;

            for (int row = 0; row < REIHEN; row++)
            {
                for (int column = 0; column < SPALTEN; column++)
                {
                    matrix[row, column] = 1;
                }
            }

            Array.Fill(vektor, 1);
            Console.WriteLine(vektor[0]);
            Console.WriteLine(matrix[0, 0]);

            //Debug.Assert(vektor.Length == matrix.Length);



            for (int i = 0; i < SPALTEN; i++)
            {
                int temp = 0;
                for (int j = 0; j < REIHEN; j++)
                {
                    temp += matrix[j, i] * vektor[i];

                }
                skalarprodukt += temp;
            }
            Console.WriteLine("Resultat matrixmultiplikation " + skalarprodukt);
            // System.Console.WriteLine(string.Join(",", result));

            DateTime end = DateTime.Now;
            Console.WriteLine("Elapsed: " + (end - dt).TotalSeconds + (" s"));

            /* wie parallelisieren ?
             * 
             * 
             * */
        }

        public static void aufgabe3()
        {  // Aufgabe 3

            DateTime dt = DateTime.Now;
            Console.WriteLine(dt.ToString());
            Console.WriteLine("Pi seriell!");
            // pi = sum i ... n 
            //double n = Math.Pow(10, 2);
            double n = 4;
            //Console.WriteLine(n);

            double pi = 0;

            double step = 1 / n;

            for (double i = 0; i < 1; i += step)
            {
                //System.Console.WriteLine("calculated" + i);
                double val = 4 / (1 + Math.Pow((i + (i + step)) / 2, 2));
                Console.WriteLine(val);
                pi = pi += (step * val);
            }

            Console.WriteLine("pi: " + pi);
            DateTime end = DateTime.Now;
            Console.WriteLine("Elapsed: " + (end - dt).TotalSeconds + (" s"));


            // optimierungen: 1 / n   teil multiplizieren ist quatsch
            // pow berechnen --> x* x
        }

        static void aufgabe3_optim()
        {  // Aufgabe 3

            DateTime dt = DateTime.Now;
            Console.WriteLine(dt.ToString());
            Console.WriteLine("Hello World!");
            // pi = sum i ... n 
            Decimal n = 1000000;
            Console.WriteLine(n);

            Decimal pi = 0;

            Decimal step = (decimal)1 / n;

            for (Decimal i = 0; i < 1; i += step)
            {
                Decimal val = 4 / (1 + ((i + i + step) / 2) * ((i + i + step) / 2));
                pi = pi += (step * val);
            }

            Console.WriteLine("pi: " + pi);
            DateTime end = DateTime.Now;
            Console.WriteLine("Elapsed: " + (end - dt).TotalSeconds + (" s"));


            // decimal anstatt double fuer genauigkeit
        }




        static int[] createArrayOfSize(int i)
        {
            int[] a = new int[i];
            Array.Fill(a, 1);
            return a;
        }
    }
}
