using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

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

        static void matrixMultParallel()
        {

        }


        public static void parallelPi_C()
        {
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
