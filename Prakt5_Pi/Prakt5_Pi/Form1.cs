using System;
using System.Collections.Generic;
using System.ComponentModel;

using System.Threading.Tasks;
using System.Windows.Forms;

using System.Diagnostics;
using System.Net;


namespace Prakt5_Pi
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        double pi_calc = 0;
        double pi_read = 0;

        //readpi
        private async void button1_Click(object sender, EventArgs e)
        {

     


            string url = "https://www.angio.net/pi/digits/50.txt";
            var client = new WebClient();
            var sw = Stopwatch.StartNew();
            string result = await client.DownloadStringTaskAsync(url);
            sw.Stop();
            Console.WriteLine(sw.ElapsedMilliseconds + " ms vergangen beim downloaden");
            result = result.Replace(".", ",");
            pi_read = double.Parse(result);
            textBox1.Text = result;

            label1.Text = sw.ElapsedMilliseconds.ToString() + " ms";

        }





        // calc pi
        private async void button2_Click(object sender, EventArgs e)
        {
            progressBar1.Value = 0;
            var sw = Stopwatch.StartNew();
            Task<double> t = new Task<double>(() =>
            {
                return Prakt4.ParallelPi_Tasks();
            });
            t.Start();
            double result = await t;
            sw.Stop();
            pi_calc = result;
            textBox2.Text = result.ToString();
            label2.Text = sw.ElapsedMilliseconds.ToString() + " ms";

            progressBar1.Value = 100;
        }



        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            if (textBox1.TextLength > 0 && textBox2.TextLength > 0)
            {
                textBox3.Text = Math.Abs(pi_calc - pi_read).ToString();
            }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            if (textBox1.TextLength > 0 && textBox2.TextLength > 0)
            {
                textBox3.Text = Math.Abs(pi_calc - pi_read).ToString();
            }
        }

      
    }
}

