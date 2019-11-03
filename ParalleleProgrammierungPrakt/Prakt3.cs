using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace ParalleleProgrammierungPrakt
{
    class Prakt3
    {

        public static void Main()
        {
            // dinersProblemNaive();
            Prakt2.parallelPi_C();
            Prakt1.aufgabe3();
        }

        private static int TOTALTHREADS = 2;



        public static void dinersProblemNaive()
        {
            List<Object> arrayOfObjects = new List<Object>(); // gabeln

            for (int i = 0; i < TOTALTHREADS; i++)
            {
                arrayOfObjects.Add(new Object());
            }
            Console.WriteLine(arrayOfObjects.Count);
            Thread[] threads = new Thread[TOTALTHREADS - 1];

            for (int i = 0; i < threads.Length; i++)
            {
                Console.WriteLine(i);
                int ii = i;
                threads[i] = new Thread(() => consumeResource(arrayOfObjects, ii));
            }

            for (int k = 0; k < threads.Length; k++)
            {
                threads[k].Start();

            }
            for (int j = 0; j < threads.Length; j++)
            {
                threads[j].Join();
            }
        }

        static void consumeResource(List<Object> objs, int numberOfThread)
        {
            int left = numberOfThread % TOTALTHREADS;
            int right = (numberOfThread + 1) % TOTALTHREADS;

            while (1 == 1)
            {

                lock (objs[left])
                {
                    Console.WriteLine("began locking left" + numberOfThread);

                    lock (objs[right])
                    {
                        Console.WriteLine("began locking right" + numberOfThread);
                        Thread.Sleep(1000);
                        Console.WriteLine("done locking " + numberOfThread);
                    }
                }
            }
        }
    }
}

