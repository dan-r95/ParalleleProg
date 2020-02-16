using System;
using System.Diagnostics;
using System.Net;
using System.Threading.Tasks;
namespace ParalleleProgrammierungPrakt
{
    class AsyncAwait
    {
        static async Task<Tuple<decimal, long>> calcPiAsync(int n)
        {
            Task<decimal> t = new Task<decimal>(() =>
            {
                decimal pi = 0;
                decimal pow;
                decimal x = 0;
                int count = 0;
                for (decimal i = 0; i < n; i++)
                {
                    x = (i - (decimal)0.5) / n;
                    pi = pi + (4 / (1 + x * x));
                    if (i % (n / 100) == 0)
                    {
                        Console.WriteLine(count + " " + n + " " + (i % (n / 100) == 0));
                        count++;
                    }
                }
                pi = pi / n;
                Console.WriteLine("pi: " + pi);
                return pi;
            });
            var sw = Stopwatch.StartNew();
            t.Start();
            decimal pi = await t;
            sw.Stop();
            Console.WriteLine("PI: " + pi);
            return new Tuple<decimal, long>(pi, sw.ElapsedMilliseconds);
        }
        static async Task<Tuple<decimal, long>> readPiAsync()
        {
            string url = "https://www.angio.net/pi/digits/50.txt";
            var client = new WebClient();
            Console.WriteLine("Requesting the data . . .");
            var sw = Stopwatch.StartNew();
            string result = await client.DownloadStringTaskAsync(url);
            decimal pi = Decimal.Parse(result, System.Globalization.CultureInfo.InvariantCulture);
            sw.Stop();
            return new Tuple<decimal, long>(pi, sw.ElapsedMilliseconds);
        }
        public static void Run()
        {
            Console.WriteLine("Hello World!");
            Task<Tuple<decimal, long>> t = calcPiAsync(1000);
            Console.WriteLine("Result-pi: " + t.Result.Item1 + " in" + t.Result.Item2 + " ms.");
            Task<Tuple<decimal, long>> t1 = readPiAsync();
            decimal pi = t1.Result.Item1;
            long time = t1.Result.Item2;
            Console.WriteLine("Downloaded pi: " + pi + "in " + time + "ms.");
            Console.WriteLine("difference abs(t1-t2): " + Math.Abs(t.Result.Item1 - t1.Result.Item1));
        }
    }
}
