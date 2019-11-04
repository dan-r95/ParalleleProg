using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Linq;

namespace ParalleleProgrammierungPrakt
{
    class Prakt2
    {

        public static void parallelPi_A()
        {
            Thread thread = new Thread(() => Prakt1.aufgabe3());


            Thread thread2 = new Thread(() => { Console.ReadLine(); thread.Abort(); Console.WriteLine("done"); });
            thread2.Start();
            thread.Start();
        }

        public static void parallelPi_B()
        {
            Thread thread = new Thread(() => Prakt1.aufgabe3());
            thread.Start();

            Thread thread2 = new Thread(() => Prakt1.aufgabe3());
            thread2.Start();
        }

        public static void matrixMultParallel()
        {
            DateTime dt = DateTime.Now;
            Console.WriteLine(dt.ToString());
            Console.WriteLine("Hello World!");

            int NUM_THREADS = 200;

            int SPALTEN = NUM_THREADS;
            int REIHEN = NUM_THREADS * 2;
            int[,] matrix = new int[REIHEN, SPALTEN];  //zeile, spalte
            int[] vektor = new int[REIHEN];  // spaltenanzahl matrix = anzahl vektorelemente
            int skalarprodukt = 0;
            object skalarprodukt_obj = (object)skalarprodukt;
            for (int row = 0; row < REIHEN; row++)
            {
                for (int column = 0; column < SPALTEN; column++)
                {
                    matrix[row, column] = 1;
                }
            }

            Array.Fill(vektor, 1);

            //Debug.Assert(vektor.Length == matrix.Length);

            Thread[] threads = new Thread[NUM_THREADS];

            for (int i = 0; i < threads.Length; i++)
            {

                int ii = i;
                threads[i] = new Thread(() =>
                {
                    int temp = 0;
                    for (int j = 0; j < REIHEN; j++)
                    {
                        temp += matrix[j, ii] * vektor[ii];

                    }
                    lock (skalarprodukt_obj)
                    {
                        skalarprodukt += temp;
                    }
                });

                threads[i].Start();
                threads[i].Join();
            }


            Console.WriteLine("Resultat matrixmultiplikation " + skalarprodukt);

            DateTime end = DateTime.Now;
            Console.WriteLine("Elapsed: " + (end - dt).TotalSeconds + (" s"));

        }


        public static void parallelPi_C()
        {
            Console.WriteLine("Paralleles pi");
            DateTime dt = DateTime.Now;

            double pi = 0;
            object pi_obj = (object)pi;

            int NUM_THREADS = 8;

            int n = 1000000;

            Thread[] threads = new Thread[NUM_THREADS];
            double[] range = Enumerable.Range(0, n + 1).Select(i => 0 + (1 - 0) * ((double)i / (n))).ToArray();
            int[] countRange = Enumerable.Range(0, NUM_THREADS + 1).Select(i => (int)(n * i) / NUM_THREADS).ToArray();


            double step = (double)1 / n;
            for (int i = 0; i < threads.Length; i++)
            {
                int ii = i;

                threads[i] = new Thread(() =>
                {

                    for (int j = countRange[ii]; j < countRange[ii + 1]; j++)
                    {
                        double leftBorder = range[j];
                        double rightBorder;

                        rightBorder = range[j + 1];

                        double val = 4 / (1 + ((leftBorder + rightBorder) / 2) * ((leftBorder + rightBorder) / 2));

                        lock (pi_obj)
                        {
                            pi = pi += (val * step);
                        }
                    }
                });
                threads[i].Start();
                threads[i].Join();
            }

            System.Console.WriteLine("Pi gesamt: " + pi);

            DateTime end = DateTime.Now;
            Console.WriteLine("Elapsed: " + (end - dt).TotalSeconds + (" s"));
        }
    }

}
