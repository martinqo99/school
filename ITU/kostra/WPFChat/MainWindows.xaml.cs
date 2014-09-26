using System.Windows;
using System.Threading;
using System.Windows.Threading;
using System;
using System.Net;
using System.IO;
using System.Data;
using System.Collections.Generic;
using Newtonsoft.Json.Serialization;
using Newtonsoft.Json;

namespace WPFChat
{
    public partial class Window1 : Window
    {
        Thread thread;        // Spuštění vlákna
        int loop = 1;
        public static Window1 myinstance = null;
        string text;
        MojaTrieda trieda;

        public Window1()
        {
            InitializeComponent();
            
            myinstance = this;
            trieda = new MojaTrieda();

            thread = new Thread(new ThreadStart(ThreadLoop));
            thread.Start();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            string user = authorTextBox.Text;
            string msg = textTextBox.Text;
            string url = string.Format("http://www.stud.fit.vutbr.cz/~xmlich02/itu-ajax/api.php?data={0}&user={1}", msg, user);

            if (!user.Equals(""))
            {
                if (!msg.Equals(""))
                {
                    HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
                    HttpWebResponse response = (HttpWebResponse)request.GetResponse();
                    response.Close();
                    authorTextBox.Text = "";
                    textTextBox.Text = "";
                }
            }
        }

        public void ThreadLoop()
        {
            while (true)
            {
                HttpWebRequest request = (HttpWebRequest)WebRequest.Create(@"http://www.stud.fit.vutbr.cz/~xmlich02/itu-ajax/api.php");
                
                // Blokující čekání na odpověď
                HttpWebResponse response = (HttpWebResponse)request.GetResponse();

                // Přečteme odpověď a uložíme do řetězce
                StreamReader reader = new StreamReader(response.GetResponseStream());
                string responseText = reader.ReadToEnd();
                response.Close();

                if (responseText.Equals(text))
                {
                }
                else
                {
                    List<ChatEntry> entries = JsonConvert.DeserializeObject<List<ChatEntry>>(responseText);
                    text = string.Copy(responseText);
                    Window1.myinstance.Dispatcher.BeginInvoke(new Action(delegate()
                    {
                        trieda.Entries.Clear();
                        foreach (ChatEntry item in entries)
                        {
                            trieda.Entries.Add(new ChatEntry() { Login = item.Login, Text = item.Text, Time = item.Time });
                        }
                        listboxik.DataContext = trieda;
                    }));
                }
                Thread.Sleep(5000);
            }
        }
    }
}
