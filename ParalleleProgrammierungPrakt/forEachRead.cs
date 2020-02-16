using System;
using System.IO;
using System.Threading.Tasks;

namespace ParalleleProgrammierungPrakt
{
    public class StreamData
    {
        public static void Run()
        {
            DateTime dt =  DateTime.Now;
            string contents;
            using (var wc = new System.Net.WebClient())
                contents = wc.DownloadString("https://raw.githubusercontent.com/mxw/grmr/master/src/finaltests/bible.txt");

            Console.WriteLine(contents.Length);
            Task<String> task = ReadCharacters(contents);
            //Task<String> task = (Task<string>)new Task(new Action(() => { }));
            String text = task.Result;

            int nVowels = 0;
            int nNonWhiteSpace = 0;
            Object obj = new Object();

            ParallelLoopResult result = Parallel.ForEach(text,
                                                        //for each character
                                                         (ch) =>
                                                         {
                                                             Char uCh = Char.ToUpper(ch);
                                                             if ("AEIOUY".IndexOf(uCh) >= 0)
                                                             {
                                                                 lock (obj)
                                                                 {
                                                                     nVowels++;
                                                                 }
                                                             }
                                                             if (!Char.IsWhiteSpace(uCh))
                                                             {
                                                                 lock (obj)
                                                                 {
                                                                     nNonWhiteSpace++;
                                                                 }
                                                             }
                                                         });
            Console.WriteLine("Total characters:      {0,10:N0}", text.Length);
            Console.WriteLine("Total vowels:          {0,10:N0}", nVowels);
            Console.WriteLine("Total non-white-space:  {0,10:N0}", nNonWhiteSpace);

            DateTime end = DateTime.Now;
            Console.WriteLine("Elapsed: " + (end - dt).TotalSeconds + (" s"));

        }


        public static Stream GenerateStreamFromString(string s)
        {
            var stream = new MemoryStream();
            var writer = new StreamWriter(stream);
            writer.Write(s);
            writer.Flush();
            stream.Position = 0;
            return stream;
        }

        private static async Task<String> ReadCharacters(String fn)
        {
            String text;

            using (var stream = GenerateStreamFromString(fn))
            {
                // ... Do stuff to stream
                using (StreamReader sr = new StreamReader(stream))
                {
                    text = await sr.ReadToEndAsync();
                }
                return text;
            }



        }
    }
    // The example displays output like the following:
    //       Total characters:         198,548
    //       Total vowels:              58,421
    //       Total non-white-space:     159,461
}