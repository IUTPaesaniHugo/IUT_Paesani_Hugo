using System.IO.Ports;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using ExtendedSerialPort;
using System.Text;
using System.Windows.Threading;
using System;
using System.Collections.Generic;
using MouseKeyboardActivityMonitor.WinApi;
using MouseKeyboardActivityMonitor;
using System.Windows.Forms;
using Utilities;
using WpfOscilloscopeControl;
using SciChart.Charting.Visuals;
using WpfAsservissementDisplay;
using Constants;
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
        private readonly KeyboardHookListener m_KeyboardHookManager;

        public MainWindow()
        {
            // Set this code once in App.xaml.cs or application startup
            SciChartSurface.SetRuntimeLicenseKey("q+sQ6lT1QvtQra3l2DyZQebLulL8S/PtlrQR6grzU7XoBL/cyIQZ6kU750dzh/NMSBZnBFqfykSR5VTTPYrRtblL6r9bp4zveRKlwtX0WWEFZLwAALTALlNaR4pSjpL2VzYhHQqXrAuos3Fofiid4FGCU1nG0EpLpofyWLSIE8jIEBAFBGh/DlFJwU4DovMaKgkQj9aA3K6D6gnkMX2xOYKbqDK0fNAZfWL80XtdfGY2MhJ6iQpsyeIHU8RBM9HcCMWQEmR5OPDTkaucGE5R+M55gAqndgbWwBVrJ9IYBlIWSobBwbW62yN3n9/kmlJCeYNJolIY8W6HozLpuMYH1LdTA8tI6fhdTrmvNyUC1ldv6VsH3nXmXWDbpsCzJ2I1u1wkcIq6+QF4yekajvHDXJueq2yq2bnOZYHIKEKx72wdrNW6P5vmU/W5Wgz6mJpcZPO2lB9/oU4gXi+tFcq56g4L32OT914UmEDQvvUcNSRG41/I66lOUPKhRrgAG+l3Mw==");

            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM11", 115200, Parity.None, 8,
                StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

            m_KeyboardHookManager = new KeyboardHookListener(new GlobalHooker());
            m_KeyboardHookManager.Enabled = true;
            m_KeyboardHookManager.KeyDown += M_KeyboardHookManager_KeyDown;
            oscilloSpeed.AddOrUpdateLine(1, 200, "Pos x");
            oscilloSpeed.ChangeLineColor(1, Color.FromRgb(0,100,255));
            oscilloSpeed.AddOrUpdateLine(3, 200, "Pos Y");
            oscilloSpeed.ChangeLineColor(3, Color.FromRgb(255, 0, 100));
            oscilloPos.AddOrUpdateLine(2, 200, "Traj.");
            oscilloPos.ChangeLineColor(2, Color.FromRgb(0, 255, 100));

        }
        private void M_KeyboardHookManager_KeyDown(object sender, KeyEventArgs e)
        {
            if (robot.autoControlActivated == false)
            {
                switch (e.KeyCode)
                {
                    case Keys.Left:
                        UartEncodeAndSendMessage(0x0051, 1, new byte[]
                        {(byte)StateRobot.STATE_TOURNE_SUR_PLACE_GAUCHE });
                        break;
                    case Keys.Right:
                        UartEncodeAndSendMessage(0x0051, 1, new byte[] {
                        (byte)StateRobot.STATE_TOURNE_SUR_PLACE_DROITE });
                        break;
                    case Keys.Up:
                        UartEncodeAndSendMessage(0x0051, 1, new byte[]
                        { (byte)StateRobot.STATE_AVANCE });
                        break;
                    case Keys.PageDown:
                        UartEncodeAndSendMessage(0x0051, 1, new byte[]
                        { (byte)StateRobot.STATE_ARRET });
                        break;
                    case Keys.Down:
                        UartEncodeAndSendMessage(0x0051, 1, new byte[]
                        { (byte)StateRobot.STATE_RECULE });
                        break;
                }
            }
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {

            while (robot.byteListReceived.Count != 0)
            {
                var c = robot.byteListReceived.Dequeue();
                //TextBoxRéception.Text += "0x" + c.ToString("X2") + " ";
                DecodeMessage(c);
            }
            if (robot.receivedText != "")
            {
                TextBoxRéception.Text += "Reçu: " + robot.receivedText + "\n";
                robot.receivedText = "";
            }
            oscilloSpeed.AddPointToLine(1, robot.moment, robot.positionXOdo);
            oscilloSpeed.AddPointToLine(3, robot.moment, robot.positionYOdo);
            oscilloPos.AddPointToLine(2, robot.positionXOdo, robot.positionYOdo);
            asservSpeedDisplay.UpdatePolarOdometrySpeed(robot.vitLinOdo, robot.vitAngOdo);
        }

        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            //robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            foreach (var b in e.Data)
                robot.byteListReceived.Enqueue(b);
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
            string s = "Bonjour";
            //byte[] array = Encoding.ASCII.GetBytes(s);
            byte[] tabled = { 0x01, 0x01};
            byte[] tabcons = { 0x41, 0x25};
            byte[] tabdist = { 0x3C, 0x14, 0x0D };
            byte[] tabtext = Encoding.ASCII.GetBytes(s);

            UartEncodeAndSendMessage(0x0020, 2, tabled);
            UartEncodeAndSendMessage(0x0040, 2, tabcons);
            UartEncodeAndSendMessage(0x0030, 3, tabdist);
            UartEncodeAndSendMessage(0x0080, 7, tabtext);
        }

        byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte checksum = 0;

            checksum ^= 0xFE;
            checksum ^= (byte)(msgFunction >> 8);
            checksum ^= (byte)(msgFunction >> 0);
            checksum ^= (byte)(msgPayloadLength >> 8);
            checksum ^= (byte)(msgPayloadLength >> 0);
            int i;
            for (i = 0; i < msgPayloadLength; i++)
            {
                checksum ^= msgPayload[i];
            }
            return checksum;
        }

        void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte[] message = new byte[msgPayloadLength + 6];
            int pos = 0;
            message[pos++] = 0xFE;
            message[pos++] = (byte)(msgFunction >> 8);
            message[pos++] = (byte)(msgFunction >> 0);
            message[pos++] = (byte)(msgPayloadLength >> 8);
            message[pos++] = (byte)(msgPayloadLength >> 0);
            int i;
            for(i=0; i<msgPayloadLength; i++)
            {
                message[pos++] = msgPayload[i];
            }

            message[pos] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

            serialPort1.Write(message, 0, msgPayloadLength + 6);
        }

        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }


        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;

        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                    }
                    break;

                case StateReception.FunctionMSB:
                    msgDecodedFunction = (c << 8);
                    rcvState = StateReception.FunctionLSB;
                    break;

                case StateReception.FunctionLSB:
                    msgDecodedFunction += c;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;

                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = (c << 8);
                    rcvState = StateReception.PayloadLengthLSB;
                    break;

                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength += c;
                    if (msgDecodedPayloadLength == 0)
                    {
                        rcvState = StateReception.CheckSum;
                    }
                    else
                    {
                        msgDecodedPayload = new byte[msgDecodedPayloadLength];
                        msgDecodedPayloadIndex = 0;
                        rcvState = StateReception.Payload;
                    }
                    break;

                case StateReception.Payload:
                    if (msgDecodedPayloadIndex < msgDecodedPayloadLength)
                    {
                        msgDecodedPayload[msgDecodedPayloadIndex] = c;
                        msgDecodedPayloadIndex++;
                        if (msgDecodedPayloadIndex == msgDecodedPayloadLength)
                        {
                            rcvState = StateReception.CheckSum;                               
                        }
                    }
                    break;

                case StateReception.CheckSum:
                    byte receivedChecksum = c;
                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    if (calculatedChecksum == receivedChecksum)
                    {
                       // TextBoxRéception.Text += "OK\n";   //Success, on a un message valide
                        ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    }
                    else
                    {
                        TextBoxRéception.Text += "Problème Checksum\n";
                    }
                    rcvState = StateReception.Waiting;
                    break;

                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }

        public enum MsgFunction
        {
            Texte=0x0080,
            Led=0x0020,
            DistanceIR=0x0030,
            ConsigneVitesse=0x0040,
            RobotState=0x0050,
            SetRobotState=0x0051,
            SetRobotManualControl=0x0052,
            PositionData=0x0061
        }

        public enum StateRobot
        {
            STATE_ATTENTE = 0,
            STATE_ATTENTE_EN_COURS = 1,
            STATE_AVANCE = 2,
            STATE_AVANCE_EN_COURS = 3,
            STATE_TOURNE_GAUCHE = 4,
            STATE_TOURNE_GAUCHE_EN_COURS = 5,
            STATE_TOURNE_DROITE = 6,
            STATE_TOURNE_DROITE_EN_COURS = 7,
            STATE_TOURNE_SUR_PLACE_GAUCHE = 8,
            STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS = 9,
            STATE_TOURNE_SUR_PLACE_DROITE = 10,
            STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS = 11,
            STATE_ARRET = 12,
            STATE_ARRET_EN_COURS = 13,
            STATE_RECULE = 14,
            STATE_RECULE_EN_COURS = 15,
            STATE_TOURNE_GAUCHE_VNR = 16,
            STATE_TOURNE_GAUCHE_EN_COURS_VNR = 17,
            STATE_TOURNE_DROITE_VNR = 18,
            STATE_TOURNE_DROITE_EN_COURS_VNR = 19
        }


        void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            switch (msgFunction)
            {
                case (int)MsgFunction.Texte:
                    robot.receivedText += Encoding.UTF8.GetString(msgDecodedPayload, 0, msgDecodedPayloadLength);

                    break;

                case (int)MsgFunction.Led:
                    switch (msgPayload[0])
                    {
                        case 0:
                            if (msgPayload[1] == 0x01)
                            {
                                CheckLed0.IsChecked = true;
                            }
                            else
                            {
                                CheckLed0.IsChecked = false;
                            }
                            break;

                        case 1:
                            if (msgPayload[1] == 0x01)
                            {
                                CheckLed1.IsChecked = true;
                            }
                            else
                            {
                                CheckLed1.IsChecked = false;
                            }
                            break;

                        case 2:
                            if (msgPayload[1] == 0x01)
                            {
                                CheckLed2.IsChecked = true;
                            }
                            else
                            {
                                CheckLed2.IsChecked = false;
                            }
                            break;
                    }

                    break;

                case (int)MsgFunction.DistanceIR:
                    robot.distanceTelemetreGauche = msgPayload[0];
                    robot.distanceTelemetreCentre = msgPayload[1];
                    robot.distanceTelemetreDroit = msgPayload[2];

                    DistanceIRGauche.Content = robot.distanceTelemetreGauche.ToString() + " cm";
                    DistanceIRCentre.Content = robot.distanceTelemetreCentre.ToString() + " cm";
                    DistanceIRDroit.Content = robot.distanceTelemetreDroit.ToString() + " cm";

                    break;

                case (int)MsgFunction.ConsigneVitesse:
                    robot.consigneGauche = msgPayload[0];
                    robot.consigneDroite = msgPayload[1];

                    VitesseGauche.Content = robot.consigneGauche.ToString() + " %";
                    VitesseDroite.Content = robot.consigneDroite.ToString() + " %";
                    break;

                case (int)MsgFunction.RobotState:
                    int instant = (((int)msgPayload[1]) << 24) + (((int)msgPayload[2]) << 16)
                    + (((int)msgPayload[3]) << 8) + ((int)msgPayload[4]);
                    TextBoxRéception.Text += "Robot␣State : " +
                    ((StateRobot)(msgPayload[0])).ToString() +
                    " - " + instant.ToString() + " ms\n";
                    break;

                case (int)MsgFunction.PositionData:
                    robot.moment = (((int)msgPayload[0]) << 24) + (((int)msgPayload[1]) << 16)
                    + (((int)msgPayload[2]) << 8) + ((int)msgPayload[3]);
                    var tab = BitConverter.ToSingle(msgPayload, 4);
                    robot.positionXOdo = tab;
                    tab = BitConverter.ToSingle(msgPayload, 8);
                    robot.positionYOdo = tab;
                    tab = BitConverter.ToSingle(msgPayload, 12);
                    robot.angleRadianOdo = tab;
                    tab = BitConverter.ToSingle(msgPayload, 16);
                    robot.vitLinOdo = tab;
                    tab = BitConverter.ToSingle(msgPayload, 20);
                    robot.vitAngOdo = tab;
                     
                    PosX.Content = robot.positionXOdo;
                    PosY.Content = robot.positionYOdo;
                    Tps.Content = robot.moment;
                    Ang.Content = robot.angleRadianOdo;
                    VitLin.Content = robot.vitLinOdo;
                    VitAng.Content = robot.vitAngOdo;
                    break;

            }
        }

        void SetRobotAutoControlState(byte state)
        {
            byte[] etat = {state};
            if (state == 1)
            {
                robot.autoControlActivated = true;
            }
            else
            {
                robot.autoControlActivated = false;
            }
            
            UartEncodeAndSendMessage(0x0052, 1, etat);
        }

        void SetRobotState(byte state)
        {
            byte[] etat = {state};
            UartEncodeAndSendMessage(0x0051, 1, etat);
        }

        byte state = 1;

        private void ModeManu_Checked(object sender, RoutedEventArgs e)
        {
            state=0;
            SetRobotAutoControlState(state);
            
        }

        private void ModeManu_Unchecked(object sender, RoutedEventArgs e)
        {
            state = 1;
            SetRobotAutoControlState(state);
        }

        private void oscilloSpeed_Loaded(object sender, RoutedEventArgs e)
        {
        }

        bool atoggle = false;
        bool btoggle = false;
        private void ButtonAsserX_Click(object sender, RoutedEventArgs e)
        {
            float kp = 0.75f;
            float ki = 1.08f;
            float kd = 0.29f;
            float pro = 0.61f;
            float propmax = 15.63f;
            float intmax = 2.34f;
            float dermax = 8.67f;

            byte[] tkp = BitConverter.GetBytes(kp);
            byte[] tki = BitConverter.GetBytes(ki);
            byte[] tkd = BitConverter.GetBytes(kd);
            byte[] tpro = BitConverter.GetBytes(pro);
            byte[] tpropmax = BitConverter.GetBytes(propmax);
            byte[] tintmax = BitConverter.GetBytes(intmax);
            byte[] tdermax = BitConverter.GetBytes(dermax);

            byte[] tabasx= new byte[29];
            tabasx[0] = 0x00;
            for (int i = 0; i < 28; i++)
            {
                if (i < 4)
                {
                    tabasx[i + 1] = tkp[i % 4];
                }
                else
                {
                    if (i < 8)
                    {
                        tabasx[i + 1] = tki[i % 4];
                    }
                    else
                    {
                        if (i < 12)
                        {
                            tabasx[i + 1] = tkd[i % 4];
                        }
                        else
                        {
                            if (i < 16)
                            {
                                tabasx[i + 1] = tpro[i % 4];
                            }
                            else
                            {
                                if (i < 20)
                                {
                                    tabasx[i + 1] = tpropmax[i % 4];
                                }
                                else
                                {
                                    if (i < 24)
                                    {
                                        tabasx[i + 1] = tintmax[i % 4];
                                    }
                                    else
                                    {
                                        tabasx[i + 1] = tdermax[i % 4];
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (atoggle == false)
            {
                ButtonAsserX.Background = Brushes.Aquamarine;
                atoggle = true;
            }
            else
            {
                ButtonAsserX.Background = Brushes.Coral;
                atoggle = false;
            }
            UartEncodeAndSendMessage(0x0063, 29, tabasx);
        }
    }
}

