using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace ParalleleProgrammierungPrakt
{
    class Prakt4
    {
      


        static void  calculateLocalSum(double leftBorder, double rightBorder)
        {
           

         
        }

        public static void parallelPi_Tasks()
        {

            Console.WriteLine("Parallel Pi Tasks");
            DateTime dt = DateTime.Now;

            CancellationTokenSource cts = new CancellationTokenSource();
            CancellationToken token = cts.Token;
            var tasks = new List<Task>();

            double pi = 0;
            object pi_obj = (object)pi;
           
            int NUM_THREADS = 4;


            double[] range = Enumerable.Range(0, NUM_THREADS ).Select(i => 0 + (1 - 0) * ((double)i / (NUM_THREADS ))).ToArray();

            // millionen mal durchlaufen lassen mit großem n, daher parallelisieren
            for (int i = 0; i < NUM_THREADS; i++)
            {
                int ii = i;
                double leftBorder = range[ii];
                double rightBorder;
                //System.Console.WriteLine("LEFT :::" + leftBorder);

                rightBorder = range[ii + 1];
                tasks.Add(
                   Task.Factory.StartNew(() =>
                   {
                       double val = 4 / (1 + ((leftBorder + rightBorder) / 2) * ((leftBorder + rightBorder) / 2));
                       Console.WriteLine(val);
                      
                       lock (pi_obj)
                       {
                           double a = (double)1 / NUM_THREADS;
                           pi = pi += (val * a);
                       }
                       if (token.IsCancellationRequested)
                           token.ThrowIfCancellationRequested();

                   }, token));
            }

            try
            {
                Task.WaitAll(tasks.ToArray());

                System.Console.WriteLine("Pi gesamt: " + pi);

                DateTime end = DateTime.Now;
                Console.WriteLine("Elapsed: " + (end - dt).TotalSeconds + (" s"));

            }
            catch (AggregateException ae)
            {
                Console.WriteLine("One or more exceptions occurred: ");
                foreach (var ex in ae.Flatten().InnerExceptions)
                    Console.WriteLine("   {0}", ex.Message);
            }



        }

        /*
       
            Console.WriteLine(string.Join(",", range));
            double step = (double)1 / NUM_THREADS;
            for (int i = 0; i < threads.Length; i++)
            {
                int ii = i;

                threads[i] = new Thread(() =>
                {

                    double leftBorder = range[ii];
                    //System.Console.WriteLine("LEFT :::" + leftBorder);
                    double rightBorder = range[ii + 1];

                    double val = 4 / (1 + ((leftBorder + rightBorder) / 2) * ((leftBorder + rightBorder) / 2));
                    lock (pi_obj)
                    {
                        double a = (double)1 / NUM_THREADS;
                        pi = pi += (val * a);
                    }
                });
                threads[i].Start();
                threads[i].Join();
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
    }*/
    }
}
