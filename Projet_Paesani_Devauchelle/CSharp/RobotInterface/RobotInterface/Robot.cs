using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RobotInterface
{
    public class Robot
    {
        public string receivedText = "";
        public float distanceTelemetreDroit;
        public float distanceTelemetreCentre;
        public float distanceTelemetreGauche;
        public bool led1;
        public bool led2;
        public bool led3;
        public int consigneGauche;
        public int consigneDroite;
        public float positionXOdo;
        public float positionYOdo;
        public float angleRadianOdo;
        public float vitLinOdo;
        public float vitAngOdo;
        public int moment;
        public Queue<byte> byteListReceived = new Queue<byte>();
        public bool autoControlActivated = true;
        public Robot()
        {
        }
    }
}
