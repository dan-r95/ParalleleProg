using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace Prakt5_Pi
{
    class Prakt5
    {
        public static async Task<double> calcPiAsync()
        {
            Task<double> t = new Task<double>(() =>
            {
                return Prakt4.ParallelPi_Tasks();
            });
            t.Start();
            double result = await t;
            return result;

        }

        public static async Task<double> readPiAsync()
        {
            string url = "https://www.angio.net/pi/digits/50.txt";
            var client = new WebClient();
            var sw = Stopwatch.StartNew();
            string result = await client.DownloadStringTaskAsync(url);
            sw.Stop();
            Console.WriteLine(sw.ElapsedMilliseconds + " ms vergangen beim downloaden");
            result = result.Replace(".", ",");
            return double.Parse(result);


        }


        public static void combineTasks()
        {
            Task<double> t = readPiAsync();
            while (!t.IsCompleted) { };
            Console.WriteLine("ergebnis download " + t.Result);

            Task<double> t2 = calcPiAsync();
            while (!t2.IsCompleted) { };
            Console.WriteLine("ergebnis kalkulation " + t2.Result);

            Task.WaitAll(t, t2);
            Console.WriteLine("fertig mit beiden aufgaben, differenz: " + Math.Abs(t.Result - t2.Result));
        }
    }
}
