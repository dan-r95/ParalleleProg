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

            //a 
            //    
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

        }


        public static void parallelPi_C()
        {
            DateTime dt = DateTime.Now;
            object balanceLock = new object();
            System.Console.WriteLine("hi");
            double pi = 0;

            int NUM_THREADS = 10;



            Thread[] threads = new Thread[NUM_THREADS];
            double[] range = Enumerable.Range(0, NUM_THREADS).Select(i => 0 + (1 - 0) * ((double)i / (NUM_THREADS - 1))).ToArray();
            Console.WriteLine(string.Join(",", range));

            for (int i = 0; i < threads.Length - 1; i++)
            {
                int ii = i;

                threads[i] = new Thread(() =>
                {
                    // lock (balanceLock)
                    // {
                        double leftBorder = range[ii];
                        double rightBorder = range[ii + 1];
                        double step = 1 / NUM_THREADS;

                        System.Console.WriteLine("left:" + leftBorder);
                        System.Console.WriteLine("right: " + rightBorder);
                        double val = 4 / (1 + Math.Pow((leftBorder + rightBorder) / 2, 2));
                        
                        System.Console.WriteLine("val: " + val);
                        System.Console.WriteLine("step: " + step);
                        System.Console.WriteLine(pi);
                        pi = pi +=  val;
                    //}
                });

            }


            for (int k = 0; k < threads.Length - 1; k++)
            {
                threads[k].Start();

            }
            for (int j = 0; j < threads.Length - 1; j++)
            {
                threads[j].Join();
            }

            System.Console.WriteLine("Pi gesamt: " + pi);

            DateTime end = DateTime.Now;
            Console.WriteLine("Elapsed: " + (end - dt).TotalSeconds + (" s"));

            // pi = 0

            //for j = linke grenze bis rechte grenze 
            // lokale summe aufsummiert


            //  lock
            //     pi =  pi + lokalesumme

            //Thread[i]  start


            // for all i    thread[i]. join



        }



    }

}
