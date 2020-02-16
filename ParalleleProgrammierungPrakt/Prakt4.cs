using System;
namespace ParalleleProgrammierungPrakt
{
    class Prakt4
    {
        public static double ParallelPi_Tasks()
        {
            Console.WriteLine("Parallel Pi Tasks");
            DateTime dt = DateTime.Now;
            CancellationTokenSource cts = new CancellationTokenSource();
            CancellationToken token = cts.Token;
            var tasks = new List<Task>();
            double pi = 0;
            object pi_obj = (object)pi;
            int NUM_TASKS = 16;
            int n = 1000000; //10e8
            Task.Factory.StartNew(() =>
                 {
                     int x = Console.Read();
                     cts.Cancel();
                     System.Console.WriteLine("Pi gesamt: tasks " + pi);
                 }, token);
            double[] range = Enumerable.Range(0, n + 1).Select(i => 0 + (1 - 0) * ((double)i / (n))).ToArray();
            int[] countRange = Enumerable.Range(0, NUM_TASKS + 1).Select(i => (int)(n * i) / NUM_TASKS).ToArray();
            double step = (double)1 / n;
            // millionen mal durchlaufen lassen mit großem n, daher parallelisieren
            for (int i = 0; i < NUM_TASKS; i++)
            {
                int ii = i;
                tasks.Add(
                   Task.Factory.StartNew(() =>
                   {
                       for (int j = countRange[ii]; j < countRange[ii + 1]; j++)
                       {
                           double leftBorder = range[j];
                           double rightBorder = range[j + 1];
                           double val = 4 / (1 + ((leftBorder + rightBorder) / 2) * ((leftBorder + rightBorder) / 2));
                           lock (pi_obj)
                           {
                               pi = pi += (val * step);
                           }
                           if (token.IsCancellationRequested)
                               token.ThrowIfCancellationRequested();
                       }
                   }, token));
            }
            try
            {
                Task.WaitAll(tasks.ToArray());
                System.Console.WriteLine("Pi gesamt tasks: " + pi);
                DateTime end = DateTime.Now;
                Console.WriteLine("Elapsed: " + (end - dt).TotalSeconds + (" s"));
                return pi;
            }
            catch (AggregateException ae)
            {
                Console.WriteLine("One or more exceptions occurred: ");
                foreach (var ex in ae.Flatten().InnerExceptions)
                    Console.WriteLine("   {0}", ex.Message);
                return 0;
            }
        }
        public static Decimal Pi_ParallelFor()
        {
            Console.WriteLine("Parallel Pi For");
            DateTime dt = DateTime.Now;
            CancellationTokenSource cts = new CancellationTokenSource();
            CancellationToken token = cts.Token;
            var tasks = new List<Task>();
            decimal pi = 0;
            object pi_obj = (object)pi;
            int NUM_THREADS = 16;
            int n = 1000000; //10e6
            bool cancellationFlag = false;
            ParallelOptions po = new ParallelOptions { CancellationToken = cts.Token };
            /*Task.Factory.StartNew(() =>
                 {
                     while (!(Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape))
                     {
                         {
                             // do something
                         }
                         cancellationFlag = true;
                         cts.Cancel();
                     }
                 }, token);
            */
            double[] range = Enumerable.Range(0, n + 1).Select(i => 0 + (1 - 0) * ((double)i / (n))).ToArray();
            int[] countRange = Enumerable.Range(0, n + 1).Select(i => (int)(n * i) / NUM_THREADS).ToArray();
            double step = (double)1 / n;
            // millionen mal durchlaufen lassen mit großem n, daher parallelisieren
            try
            {
                var l = new Object();
                decimal x = 0;
                decimal y = 0;
                Parallel.For(0, n, i =>
                          {
                              x = (i - (decimal)0.5) / n;
                              y = (4 / (1 + x * x));
                              lock (l)
                              {
                                  pi = pi + y;
                              }
                          });
                pi = pi / n;
                // hier fehlt noch sämtliche parallelität
                /*
                Parallel.For<int>(10, 100, () => { return 0; },
                (i, loop, localState) => {
                    localState += (int)i % 2;
                    return localState;
                },
                z => {
                    lock (myLock) result = (result + z) % 42;
                }
                );*/
                System.Console.WriteLine("Pi parallelfor: " + pi);
                DateTime end = DateTime.Now;
                Console.WriteLine("Elapsed: " + (end - dt).TotalSeconds + (" s"));
                return pi;
            }
            catch (OperationCanceledException e)
            {
                Console.WriteLine(e.Message);
                return 0;
            }
            finally
            {
                cts.Dispose();
            }
        }
    }
}
