using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace ParalleleProgrammierungPrakt
{
    class Prakt3Priorisiert
    {
        private static int TOTALTHREADS = 2;


        //private static int priority 
        // resource 20 ... prio 20 
        // resourrce 1... prio 1

        public static void dinersProblemSmart()
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
                if (left > right)
                {

                    // z b gabel mit kleinerem index zuerst aufnehmen
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
                if (left < right)
                {
                    // z b gabel mit kleinerem index zuerst aufnehmen
                    lock (objs[right])
                    {
                        Console.WriteLine("began locking left" + numberOfThread);

                        lock (objs[left])
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
}


