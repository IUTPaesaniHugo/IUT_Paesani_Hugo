using System.IO.Ports;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using ExtendedSerialPort;
using System.Text;
using System.Windows.Threading;
using System;
using System.Collections.Generic;

namespace RobotInterface
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>
    /// 
    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        Robot robot = new Robot();

        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM3", 115200, Parity.None, 8,
                StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            //if (robot.receivedText != "")
            //{
            //    TextBoxRéception.Text += "Reçu: " + robot.receivedText;
            //    robot.receivedText = "";
            //}

            while (robot.byteListReceived.Count != 0)
            {
                var c = robot.byteListReceived.Dequeue();
                TextBoxRéception.Text += "0x" + c.ToString("X2") + " ";

            }
        }

        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            //robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            foreach (var b in e.Data)
                robot.byteListReceived.Enqueue(b);

            //for (int i=0; i<e.Data.Length; i++)
            //{
            //    Robot.byteListReceived.Enqueue(e.Data[i]);
            //}
            
        }

        private void TextBoxRéception_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        bool toggle = false;


        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            if (toggle == false)
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
                toggle = true;
            }
            else
            {
                buttonEnvoyer.Background = Brushes.Beige;
                toggle = false;
            }

            sendMessage(1);
        }

        private void TextBoxEmission_KeyUp(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == System.Windows.Input.Key.Enter)
            {
                sendMessage(0);
            }
        }

        private void sendMessage(int dou)
        {
            if ((TextBoxEmission.Text != "") && (TextBoxEmission.Text != "\r\n"))
            {
                serialPort1.WriteLine(TextBoxEmission.Text);
                if (dou == 1)
                {
                }
            }
            TextBoxEmission.Text = "";
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            TextBoxRéception.Text = "";
        }

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            byte [] byteList=new byte[20];
            for(int i=0; i<20; i++)
            {
                byteList[i] = (byte)(2 * i);
            }
            serialPort1.Write(byteList, 0, 20);
        }
    }
}
