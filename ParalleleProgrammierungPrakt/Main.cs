namespace ParalleleProgrammierungPrakt
{

    class MainClass
    {

        public static void Main()
        {
            Prakt1.aufgabe3();
            // dinersProblemNaive();
            Prakt2.parallelPi_C();

            //Prakt2.matrixMultParallel();

            Prakt4.ParallelPi_Tasks();
            Prakt4.Pi_ParallelFor();
            StreamData.Run();

            //Prakt4.Pi_ParallelFor();
            ForEachWithThreadLocal.Run();
            RandomBreak.Run();
            Summary.Run();
            AsyncAwait.Run();

            Prakt5.combineTasks();
        }
    }

}