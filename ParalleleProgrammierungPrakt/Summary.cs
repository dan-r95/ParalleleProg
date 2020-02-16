using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace ParalleleProgrammierungPrakt
{
    class Summary
    {
        public static void Run()
        {
            Console.WriteLine("Hello World!");
            //aufgabe1();
            //aufgabe2();
            //simpleThread();
            //simpleLambdaDelegate();
            //simpleMonitor();
            //simpleLock();
            //diningPhilo();
            //simpleTask();
            //simpleTaskAbort();
            //simpleParallelFor();
            //simpleParallelFor2_0();
            simpleAsyncAwaitAsync();
            Console.WriteLine("Hallo");
        }

        private static async void simpleAsyncAwaitAsync()
        {
            string result = await WaitSynchronously();
            string result2 = await WaitAsynchronouslyAsync();
            Console.WriteLine(result + "r");

            async Task<string> WaitAsynchronouslyAsync()
            {
                await Task.Delay(10000);
                return "Finished";
            }

            async Task<string> WaitSynchronously()
            {
                // Add a using directive for System.Threading.
                Thread.Sleep(10000);
                return "Finished";
            }

        }

        private static void simpleParallelFor2_0()
        {
            Object myLock = new Object();
            int result = 1;
            Parallel.For<int>(10, 100, () => { return 0/*Startwert für jeden einzelnen lokalen Lauf[wird in in x geschrieben]*/; },
                (i, pls, x) =>//i wird in einer Schleife von in diesem Fall 1 bis 100 iteriert,pls nicht wichtig, x: siehe oben 
                {
                    //Console.WriteLine("i ist:" + i);
                    //Console.WriteLine("x ist:" + x);
                    //Console.WriteLine("pls ist:" + pls);
                    x = x + 1;
                    return x;
                },
                z =>
                {
                    lock (myLock)
                    {
                        //Console.WriteLine("result old()tmp ist:" + result);
                        // z ist das Resultat eines jeden lokalen Durchlaufs
                        result = (result + z);
                        //Console.WriteLine("result new()tmp ist:" + result);
                    }
                }

            );

            Console.WriteLine("result ist:" + result);
        }
        private static void simpleParallelFor()
        {
            Parallel.For(0, 5, i =>
            {
                Console.WriteLine("Durchlauf mit i = {0}", i);
                Thread.Sleep(1000);
            });

            Parallel.ForEach(new int[] { 1, 2, 3, 4, 5 }, i =>
                 {
                     Console.WriteLine("Durchlauf mit i = {0}", i);
                     Thread.Sleep(1000);
                 });
            Console.WriteLine();
        }

        private static void simpleTaskAbort()
        {
            var tokenSource = new CancellationTokenSource();
            Task job = new Task(() =>
            {
                for (int i = 0; i < 10; i++)
                {
                    Thread.Sleep(100); // heavy work is going on here
                    tokenSource.Token.ThrowIfCancellationRequested();
                }
                Console.WriteLine("Job ist fertig");
            }, tokenSource.Token);
            Action<Task> success = new Action<Task>((successTask) =>
            {
                Console.WriteLine("Task erfolgreich beendet.", successTask.Id);
            });
            Action<Task> failed = new Action<Task>((o) =>
            {
                Console.WriteLine("Job fehlerhaft beendet.");
            });
            job.ContinueWith(success, TaskContinuationOptions.OnlyOnRanToCompletion);
            job.ContinueWith(failed, TaskContinuationOptions.OnlyOnCanceled);
            job.Start();
            tokenSource.Cancel();
            try { job.Wait(); }
            catch { }
        }
        private static void simpleTask()
        {

            Console.WriteLine(1);
            Task<int> t = Task.Factory.StartNew(() =>
            {
                Thread.Sleep(1000);
                return 2;
            });


            Console.WriteLine(t.Result);
            Console.WriteLine(3);

            Task<int> t2 = new Task<int>(() =>
            {
                Thread.Sleep(1000);
                return 4;
            });
            t2.Start();

            Console.WriteLine(t2.Result);

            Task<int> t3 = new Task<int>(simpleFun);
            t3.Start();

            int simpleFun()
            {
                Thread.Sleep(1000);
                return 5;
            }
            t3.Wait();
            Task.WaitAll(/*Liste von Tasks/Array von Tasks*/);
            Task.WaitAny();//wie oben
            Task.WaitAny(new Task[] { t, t2 });

            Console.WriteLine(t3.Result);
        }



        private static void diningPhilo()
        {
            int n = 2;
            //static bool[] gabeln = new bool[n];
            object[] gabeln = new object[n];

            //Console.WriteLine("Hallo {0}", "df");
            //return;
            for (int i = 0; i < gabeln.Length; i++)
            {
                gabeln[i] = new object();
            }
            Console.WriteLine("Hello World!");

            Thread[] threads = new Thread[n];

            //Console.WriteLine(gabeln[30]);
            Console.WriteLine(gabeln[1]);
            for (int i = 0; i < threads.Length; i++)
            {

                int ii = i;
                threads[i] = new Thread(() =>
                {
                    while (true)
                    {
                        int linGabIndex = i == 0 ? n : i - 1;
                        int rechGabIndex = i == n ? 0 : i + 1;
                        rechGabIndex = ii % n;
                        linGabIndex = (ii + 1) % n;
                        //linke Gabel nehmen
                        int prio1 = linGabIndex;
                        int prio2 = rechGabIndex;
                        if (ii == threads.Length - 1)
                        {
                            prio1 = rechGabIndex;
                            prio2 = linGabIndex;
                        }



                        lock (gabeln[prio1])
                        {
                            Console.WriteLine("Person " + ii + " benutzt nun die Gabel (" + prio1 + ").");
                            lock (gabeln[prio2])
                            {
                                Console.WriteLine("Person " + ii + " benutzt nun die Gabel (" + prio2 + ").");
                                Thread.Sleep(1000);
                            }
                        }
                        Console.WriteLine("Person " + ii + " hat beide Gabeln zurückgegeben.");
                    }




                });
            }


            for (int i = 0; i < threads.Length; i++)
            {
                threads[i].Start();
            }

            for (int i = 0; i < threads.Length; i++)
            {
                threads[i].Join();
            }

        }

        private static void simpleLock()
        {
            object lockObject = new object();
            int v = 0;

            Thread[] threadArray = new Thread[10];

            for (int i = 0; i < threadArray.Length; i++)
            {
                threadArray[i] = new Thread(() => addOne());
            }

            for (int i = 0; i < threadArray.Length; i++)
            {
                threadArray[i].Start();
            }

            for (int i = 0; i < threadArray.Length; i++)
            {
                threadArray[i].Join();

            }

            Console.WriteLine("V ist: " + v);

            void addOne()
            {
                lock (lockObject)
                {
                    v = v + 1;
                }
            }
        }

        private static void simpleMonitor()
        {
            object lockObject = new object();
            int v = 0;

            Thread[] threadArray = new Thread[10];

            for (int i = 0; i < threadArray.Length; i++)
            {
                threadArray[i] = new Thread(() => addOne());
            }

            for (int i = 0; i < threadArray.Length; i++)
            {
                threadArray[i].Start();
            }

            for (int i = 0; i < threadArray.Length; i++)
            {
                threadArray[i].Join();

            }

            Console.WriteLine("V ist: " + v);

            void addOne()
            {
                Monitor.Enter(lockObject);
                v = v + 1;
                Monitor.Exit(lockObject);
            }
        }

        private static void simpleLambdaDelegate()
        {
            int[] myArray = new int[] { 13, 39, 42, 53, 72, 23, 65 };
            var tmp = Array.FindAll(myArray, x => x > 30);

            foreach (var item in tmp)
            {
                Console.WriteLine(item);
            }
        }

        private static void simpleThread()
        {
            //ThreadStart del = new ThreadStart(myParallelMethod);
            //Thread thread = new Thread(del);
            //thread.Start();

            //Alternativ
            Thread thread2 = new Thread(() => myParallelMethod());
            thread2.Start();
            thread2.Join();
            Console.WriteLine("3");
        }

        private static void myParallelMethod()
        {
            Console.WriteLine("1");
            Thread.Sleep(1000);
            Console.WriteLine("2");
        }

        private static void aufgabe2()
        {
            int n = 100;
            int[,] matrix = new int[n, n];
            int[] v1 = new int[n];
            int[] v2 = new int[n];
            for (int i = 0; i < n; i++)
            {
                v1[i] = 1;
                for (int j = 0; j < n; j++)
                {
                    matrix[i, j] = 1;
                }
            }
            for (int i = 0; i < n; i++)
            {
                int sum = 0;
                for (int j = 0; j < n; j++)
                {
                    sum += matrix[i, j] * v1[j];
                }
                v2[i] = sum;
            }

            for (int i = 0; i < v2.Length; i++)
            {
                Console.WriteLine(v2[i] + "\n");
            }




        }

        private static void aufgabe1()
        {
            int n = 1000000;
            int[] v1 = new int[n];
            int[] v2 = new int[n];
            int[] v3 = new int[n];
            List<int> parts = new List<int>();

            parts.ElementAt(0);



            LinkedList<int> sentence = new LinkedList<int>();
            sentence.ElementAt(0);


            for (int i = 0; i < v1.Length; i++)
            {
                v2[i] = 1;
                v1[i] = 1;
            }

            var sum = 0;
            for (int i = 0; i < v1.Length; i++)
            {
                var item = v1[i];
                var item2 = v2[i];
                var tmp = item * item2;
                sum = sum + tmp;

            }

            Console.WriteLine(sum + "     IST SO ");

            Console.WriteLine(sum + "     IST SO ");
        }
    }
}
