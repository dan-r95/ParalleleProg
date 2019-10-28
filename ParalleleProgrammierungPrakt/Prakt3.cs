using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace ParalleleProgrammierungPrakt
{
    class Prakt3
    {
        private static int numThreads = 5;

        static void Main(string[] args)
        {
            //aufgabe1();

            //Aufgabe 2
            //aufgabe2();
            //aufgabe3();
            // aufgabe3_optim();
            //Prakt2.parallelPi_B();
            dinersProblemNaive();
        }



        public static void dinersProblemNaive()
        {




            List<Object> arrayOfObjects = new List<Object>();// gabeln

            for (int i = 0; i < numThreads; i++)
            {
                arrayOfObjects.Add(new Object());
            }
            Console.WriteLine(arrayOfObjects.Count);
            Thread[] threads = new Thread[numThreads];

            for (int i = 0; i < numThreads; i++)
            {
                Console.WriteLine(i);
                threads[i] = new Thread(() => consumeResource(arrayOfObjects, i));


                threads[i].Start();
                threads[i].Join();
            }
        }

        static void consumeResource(List<Object> objs, int numberOfThread)
        {
            int left = numberOfThread - 1;
            int right = numberOfThread + 1;
            if (numberOfThread == 0)
            {
                left = numberOfThread - 1;
            }
            else if (numberOfThread == numThreads - 1)
            {
                right = 0;
            }

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

